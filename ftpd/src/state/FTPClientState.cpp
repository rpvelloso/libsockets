/*
 * FTPClientState.h
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
 */

#include "state/FTPClientState.h"

std::unordered_map<FTPReply, std::string> FTPReplyString= {
		{FTPReply::R150			,"150 Opening data connection.\n"},
		{FTPReply::R200			,"200 Command successful.\n"},
		{FTPReply::R200_MODE	,"200 Mode set to S.\n"},
		{FTPReply::R200_TYPE	,"200 Type set to "},
		{FTPReply::R200_STRU	,"200 Structure set to F.\n"},
		{FTPReply::R211			,"211 Help not available.\n"},
		{FTPReply::R213			,"213 "},
		{FTPReply::R215			,"215 UNIX Type: L8.\n"},
		{FTPReply::R220			,"220 Server ready.\n"},
		{FTPReply::R221			,"221 Connection ended.\n"},
		{FTPReply::R226			,"226 Transfer complete.\n"},
		{FTPReply::R227			,"227 Entering Passive Mode ("},
		{FTPReply::R230			,"230 User logged in.\n"},
		{FTPReply::R250			,"250 Command successful.\n"},
		{FTPReply::R257			,"257 Directory successfully created.\n"},
		{FTPReply::R257_PWD		,"257 \""},
		{FTPReply::R331			,"331 Password required.\n"},
		{FTPReply::R350			,"350 Restarted position accepted.\n"},
		{FTPReply::R350_RNFR	,"350 Rename from accepted.\n"},
		{FTPReply::R421			,"421 Server full.\n"},
		{FTPReply::R425			,"425 Can't open data connection.\n"},
		{FTPReply::R426			,"426 Connection closed} transfer aborted.\n"},
		{FTPReply::R500			,"500 Invalid command.\n"},
		{FTPReply::R501			,"501 Invalid arguments.\n"},
		{FTPReply::R503			,"503 User name required.\n"},
		{FTPReply::R503_RNTO	,"503 Bad sequence of commands.\n"},
		{FTPReply::R504			,"504 Unsupported parameter.\n"},
		{FTPReply::R530			,"530 Not logged in.\n"},
		{FTPReply::R550_MKD		,"550 Could not create directory.\n"},
		{FTPReply::R550_SIZE1	,"550 Not a regular file.\n"},
		{FTPReply::R550_SIZE2	,"550 No such file or directory.\n"},
		{FTPReply::R550_CWD		,"550 Can't change directory.\n"},
		{FTPReply::R550_RETR	,"550 Can't send file.\n"},
		{FTPReply::R550_STOR	,"550 Can't receive file.\n"},
		{FTPReply::R550_DELE	,"550 Can't delete file.\n"},
		{FTPReply::R550_RNTO	,"550 Can't rename file.\n"}
};

FTPClientState::FTPClientState(FTPContext &ctx) : context(ctx) {};
FTPClientState::~FTPClientState() {};
FTPReply FTPClientState::NOOP() { return FTPReply::R200; };
FTPReply FTPClientState::QUIT() { return FTPReply::R221; };
FTPReply FTPClientState::USER(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::PASS(const std::string &) { return FTPReply::R500; };

FTPReply FTPClientState::MODE(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::STRU(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::TYPE(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::CWD(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::MKD(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::SIZE(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::HELP() { return FTPReply::R500; };
FTPReply FTPClientState::SYST() { return FTPReply::R500; };
FTPReply FTPClientState::PWD() { return FTPReply::R500; };
FTPReply FTPClientState::DELE(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::RNFR(const std::string &) { return FTPReply::R503_RNTO; };
FTPReply FTPClientState::RNTO(const std::string &) { return FTPReply::R503_RNTO; };

FTPReply FTPClientState::LIST(const std::string &, int) { return FTPReply::R500; };
FTPReply FTPClientState::RETR(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::STOR(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::REST(const std::string &) { return FTPReply::R500; };

FTPReply FTPClientState::PORT(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::PASV() { return FTPReply::R500; };
