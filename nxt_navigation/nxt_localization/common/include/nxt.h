/*	nxt.h (still under development)
 * 	Authors: Christian A. Mueller, Paul G. Ploeger
 *  Modified by: Frederik Hegger
 *
 *	Copyright 2011
 *
 *  This file is part of NXTLocalizationSystem.
 *
 *   NXTLocalizationSystem is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   NXTLocalizationSystem is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with NXTLocalizationSystem.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NXT_H
#define NXT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <malloc.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>

#define  MAX_MESSAGE_SIZE 64
//64

class Nxt
{
private:
	int nxtSocket;
	std::string btAddressNxt;
	bool isConn;
public:
	Nxt();
	bool findAndConnectNxt();
	std::vector< std::pair< std::string,std::string> > findNxt(char addr[19]);
	int connectNxt(char *btAddress);
	int disconnectNxt();
	int sendMessage(int mbox, char *message);
	int readMessage(int mbox, std::string &strMessage);
	bool isConnected();
	std::string getBTAddressNxt();
};

#endif





