/****************************************************************************
 **
 ** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the demonstration applications of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL21$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia. For licensing terms and
 ** conditions see http://qt.digia.com/licensing. For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 or version 3 as published by the Free
 ** Software Foundation and appearing in the file LICENSE.LGPLv21 and
 ** LICENSE.LGPLv3 included in the packaging of this file. Please review the
 ** following information to ensure the GNU Lesser General Public License
 ** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
 ** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights. These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include "browserapplication.h"

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

/** fbc: headers for fbc **/
#include "fbc.h"
#include "browsermainwindow.h"
#include <sys/types.h>

//#include <fcntl.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>

BrowserApplication *application;

char fbc_url_requested[FBC_MAX_URL_REQUESTED] = {'\0', };	
bool fbc_failed = true;
pthread_mutex_t mtx_fbc_failed = PTHREAD_MUTEX_INITIALIZER;

int fbc_tty_settings[2];

#ifdef FBC_ENABLED
pthread_t fbc_t_capturer;

void *tfunc_fbcapture(void *data);
void sigH_fbcapture(int signo);
#endif 

void fbc_direct_setting();
void fbc_lazy_resetting();
void sigH_tty_resetting(int signo);

int main(int argc, char **argv)
{
	signal(SIGTERM, sigH_tty_resetting);
	fbc_direct_setting();
#ifdef FBC_ENABLED
	strcpy(fbc_url_requested, (char *)argv[argc-1]);
	pthread_create(&fbc_t_capturer, NULL, tfunc_fbcapture, NULL); 
	pthread_detach(fbc_t_capturer);

	signal(SIGUSR1, sigH_fbcapture);
	//signal(SIGINT, sigH_fbcapture);
#endif

	Q_INIT_RESOURCE(data);

	application = new BrowserApplication(argc, argv);
	if (!(application->isTheOnlyBrowser()) || !(application->isCorrectlyInitialized()))
		return 0;

	application->newMainWindow();

	return application->exec();
}

#ifdef FBC_ENABLED
void *tfunc_fbcapture(void *data)
{

	if(!fbc_restore(fbc_url_requested))
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "Failed to restore bitmap.");
#endif
		pthread_mutex_lock(&mtx_fbc_failed);
		fbc_failed = true;
		pthread_mutex_unlock(&mtx_fbc_failed);
	}
	else
	{
#ifdef FBC_DEBUG_TIME_ENABLED
		fbc_debug_time("WebPageLoaded");
		fflush(stdout);
#endif
		pthread_mutex_lock(&mtx_fbc_failed);
		fbc_failed = false;
		pthread_mutex_unlock(&mtx_fbc_failed);
	}

	while(true)
		pause();

	return (void *)true;
}

void sigH_fbcapture(int signo)
{
	if(signo == SIGUSR1) /** fbc: loadFinished() was called. **/
	{
		if(pthread_self() == fbc_t_capturer) /** fbc: in restoration thread **/
		{
			usleep(200000); /** fbc: sleeping during 200ms to wait painting **/
#ifdef FBC_DEBUG_ENABLED
			if(!fbc_store(fbc_url_requested))
			{
				fbc_debug(__func__, "Failed to store bitmap.");
			}

#else
			fbc_store(fbc_url_requested);
#endif
		}

		else /** fbc: in web page processing thread **/
		{
			application->mainWindow()->setVisible(true);
		}
	}
}
#endif

void fbc_direct_setting()
{
	int fbc_tty_fd = open("/dev/tty1", O_RDWR);

	ioctl(fbc_tty_fd, KDGETMODE, &fbc_tty_settings[FBC_TTY_SETTING_MODE]);
	ioctl(fbc_tty_fd, KDGKBMODE, &fbc_tty_settings[FBC_TTY_SETTING_KEYBOARD]);
	close(fbc_tty_fd);

	atexit(fbc_lazy_resetting);
}

void fbc_lazy_resetting()
{
	int fbc_tty_fd = open("/dev/tty1", O_RDWR);
	const char fbc_tty_reset_msg[] = "\033[9;15]\033[?33h\033[?25h\033[?0c";

	ioctl(fbc_tty_fd, KDSETMODE, fbc_tty_settings[FBC_TTY_SETTING_MODE]);
	ioctl(fbc_tty_fd, KDSKBMODE, fbc_tty_settings[FBC_TTY_SETTING_KEYBOARD]);

	write(fbc_tty_fd, fbc_tty_reset_msg, sizeof(fbc_tty_reset_msg));

	close(fbc_tty_fd);
}

void sigH_tty_resetting(int signo)
{
	if(signo == SIGTERM)
	{
		exit(0);
	}
}
