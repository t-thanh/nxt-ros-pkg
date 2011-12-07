/*	nxt.cpp (still under development)
 * 	Authors: Christian A. Mueller, Paul G. Ploeger
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

#include <nxt.h>
#include <algorithm>
#include <utility>
#include <map>

/*
 * Information can be found under
 * http://people.csail.mit.edu/albert/bluez-intro/
 * */

Nxt::Nxt() {
	btAddressNxt = "-";
	this->isConn = false;
}

bool Nxt::findAndConnectNxt() {
	char addr[19];
	this->findNxt(addr);
	if (this->connectNxt(addr) == 0) {
		return true;
	}

	return false;
}

bool Nxt::isConnected() {
	return this->isConn;
}

std::string Nxt::getBTAddressNxt() {

	return btAddressNxt;
}

std::vector<std::pair<std::string, std::string> > Nxt::findNxt(char addr[19]) {
	std::cout << "Nxt finding...\n";
	std::vector<std::pair<std::string, std::string> > foundDevs;

	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	inquiry_info *inquiryInfo = NULL;
	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	inquiryInfo = (inquiry_info*) malloc(max_rsp * sizeof(inquiry_info));

	char name[248] = { 0 };

	dev_id = hci_get_route(NULL);
	sock = hci_open_dev(dev_id);
	if (dev_id < 0 || sock < 0) {
		perror("Error Opening socket(hci_open_dev)");
	}

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &inquiryInfo, flags);
	if (num_rsp < 0)
		perror("Error hci_inquiry");

	for (i = 0; i < num_rsp; i++) {
		std::pair<std::string, std::string> currDev;
		ba2str(&(inquiryInfo + i)->bdaddr, addr);
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name(sock, &(inquiryInfo + i)->bdaddr,
				sizeof(name), name, 0) < 0)
			strcpy(name, "[unknown]");
		printf("Found: %s  %s\n", addr, name);
		currDev.first = std::string(addr);
		currDev.second = std::string(name);
		foundDevs.push_back(currDev);
	}

	free(inquiryInfo);
	close(sock);

	return foundDevs;
}

int Nxt::connectNxt(char *btAddress) {
	struct sockaddr_rc addr = { 0 };
	int status;
	printf("Connect to %s \n", btAddress);

	nxtSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	str2ba(btAddress, &addr.rc_bdaddr);

	status = connect(nxtSocket, (struct sockaddr *) &addr, sizeof(addr));
	if (status < 0) {
		perror("Error connecting Bluetooth");
		return status;
	}

	this->btAddressNxt = std::string(btAddress);
	isConn = true;
	return 0;
}

int Nxt::disconnectNxt() {
	close(nxtSocket);
	btAddressNxt = "-";
	isConn = false;
	return 0;
}

int Nxt::sendMessage(int mbox, char *message) {

	if (this->isConnected() == false) {
		return -1;
	}

	unsigned char btlength[2] = { 0x00, 0x00 };
	unsigned char cmd[MAX_MESSAGE_SIZE] = { 0x0 };
	unsigned char reply[MAX_MESSAGE_SIZE] = { 0x0 };
	int result, msgsize, replylength;
	int error = 0;

	msgsize = strlen(message) + 1; // add one for null termination
	if (msgsize > (MAX_MESSAGE_SIZE - 4)) {
		fprintf(stderr, "messagewrite : message is to long");
		return -1;
	}
	// Message send to nxt consists of follwoing commands
	cmd[0] = 0x00;//08
	cmd[1] = 0x09;
	cmd[2] = mbox;
	cmd[3] = msgsize;
	memcpy(&cmd[4], message, msgsize);
	// bluetooth message length
	btlength[0] = 4 + msgsize;

	if ((result = write(nxtSocket, btlength, 2)) < 0) {
		perror("Error message sending while sending bluetooth length");
		return result;
	}
	if ((result = write(nxtSocket, cmd, btlength[0])) < 0) {
		perror("Error message sending while sending command ");
		return result;
	}
	if ((result = read(nxtSocket, reply, 2)) < 0) {
		perror("Error message sending while receiving message reply(1)");
		return result;
	}
	replylength = reply[0] + (reply[1] * 256);
	if ((result = read(nxtSocket, reply, replylength)) < 0) {
		perror("Error message sending while receiving message reply(2)");
		return result;
	}

	if (replylength != result) {
		fprintf(
				stderr,
				"Error message sending while checking length of msg, msg with different lengths  : %d != %d\n",
				replylength, result);
	}

	//checking reply
	//checking byte 0
	if (reply[0] != 0x02) {
		fprintf(stderr, "Error message sending (byte 0) -> %hhx != 0x02\n",
				reply[0]);
		error = 1;
	}
	// byte 1
	if (reply[1] != 0x09) {
		fprintf(stderr, "Error message sending (byte 1) -> %hhx != 0x13\n",
				reply[1]);
		error = 1;
	}
	// byte 2
	if (reply[2] != 0x00) {
		fprintf(stderr, "Error message sending (byte 2) ->  %hhx \n", reply[2]);
		error = 1;
	}
	if (error == 1) {
		return -1;
	}
	return 0;
}

int Nxt::readMessage(int mbox, std::string &strMessage) {
	if (this->isConnected() == false) {
		return -1;
	}
	char *message = NULL;
	unsigned char btlength[2] = { 0x00, 0x00 };
	unsigned char cmd[5] = { 0x0 };
	unsigned char reply[MAX_MESSAGE_SIZE];
	int result, cmdlength, msgsize;
	int error = 0;

	// creating nxt command for reading message from mailbox;
	cmd[0] = 0x00;
	cmd[1] = 0x13;
	cmd[2] = mbox + 10;
	cmd[3] = 0x00;
	cmd[4] = 0x01;

	// message is 5 long
	btlength[0] = 5;

	// sending bluetooth length
	if ((result = write(nxtSocket, btlength, 2)) < 0) {
		perror("Error reading message command(1) ");
		return result;
	}

	// sending reading command
	if ((result = write(nxtSocket, cmd, 5)) < 0) {
		perror("Error reading message command(2) ");
		return result;
	}

	// get bluetooth message length
	if ((result = read(nxtSocket, reply, 2)) < 0) {
		perror("Error reading message command(3)");
		return result;
	}

	cmdlength = reply[0] + (reply[1] * 256);
	// get reply
	if ((result = read(nxtSocket, reply, cmdlength)) < 0) {
		perror("Error reading message command(4) ");
		return result;
	}

	if (cmdlength != result) {
		fprintf(
				stderr,
				"Error reading message while checking length of msg, %d != %d\n",
				cmdlength, result);
	}
	if (reply[0] != 0x02) {
		// byte 0
		fprintf(stderr, "Error message reading (byte 0) -> %hhx != 0x02\n",
				reply[0]);
		error = 1;
	}
	if (reply[1] != 0x13) {
		// byte 1
		fprintf(stderr, "Error message reading (byte 1) -> %hhx != 0x13\n",
				reply[1]);
		error = 1;
	}
	if (reply[2] == 0x40) {
		// byte 2
		printf("mailbox empty\n");
		return reply[2];
	}
	if (reply[2] != 0x00) {
		fprintf(stderr, "Error message reading (byte 2) -> %hhx \n", reply[2]);
		error = 1;
	}
	if (reply[3] != 0x00) {
		// byte 3
		fprintf(stderr, "Error message reading (byte 3) -> %hhx != 0x00\n",
				reply[3]);
		error = 1;
	}
	if (error) {
		return -1;
	}
	msgsize = reply[4];
	message = (char *) malloc(sizeof(char) * msgsize + 2);
	printf("Message Reply [5-%d]: %s\n", msgsize + 6, &reply[5]);
	// byte 5-63: message data
	memcpy(message, &reply[5], msgsize);
	printf("Message received: %s\n", message);

	strMessage = std::string(message);
	return 0;
}
