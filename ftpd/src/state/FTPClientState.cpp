/*
 * FTPClientState.h
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
 */

#include "state/FTPClientState.h"

std::unordered_map<FTPReply, std::string> FTPReplyString= {
		{FTPReply::R150			,"150 Opening data connection."},
		{FTPReply::R200			,"200 Command successful."},
		{FTPReply::R200_MODE	,"200 Mode set to S."},
		{FTPReply::R200_TYPE	,"200 Type set to "},
		{FTPReply::R200_STRU	,"200 Structure set to F."},
		{FTPReply::R211			,"211 Help not available."},
		{FTPReply::R213			,"213 "},
		{FTPReply::R215			,"215 UNIX Type: L8."},
		{FTPReply::R220			,"220 Server ready."},
		{FTPReply::R221			,"221 Connection ended."},
		{FTPReply::R226			,"226 Transfer complete."},
		{FTPReply::R227			,"227 Entering Passive Mode ("},
		{FTPReply::R230			,"230 User logged in."},
		{FTPReply::R250			,"250 Command successful."},
		{FTPReply::R257			,"257 Directory successfully created."},
		{FTPReply::R257_PWD		,"257 \""},
		{FTPReply::R331			,"331 Password required."},
		{FTPReply::R350			,"350 Restart position accepted."},
		{FTPReply::R350_RNFR	,"350 Rename from accepted."},
		{FTPReply::R421			,"421 Server full."},
		{FTPReply::R425			,"425 Can't open data connection."},
		{FTPReply::R426			,"426 Connection closed} transfer aborted."},
		{FTPReply::R500			,"500 Invalid command."},
		{FTPReply::R501			,"501 Invalid arguments."},
		{FTPReply::R503			,"503 User name required."},
		{FTPReply::R503_RNTO	,"503 Bad sequence of commands."},
		{FTPReply::R504			,"504 Unsupported parameter."},
		{FTPReply::R530			,"530 Not logged in."},
		{FTPReply::R550_MKD		,"550 Could not create directory."},
		{FTPReply::R550_SIZE1	,"550 Not a regular file."},
		{FTPReply::R550_SIZE2	,"550 No such file or directory."},
		{FTPReply::R550_CWD		,"550 Can't change directory."},
		{FTPReply::R550_RETR	,"550 Can't send file."},
		{FTPReply::R550_STOR	,"550 Can't receive file."},
		{FTPReply::R550_DELE	,"550 Can't delete file."},
		{FTPReply::R550_RNTO	,"550 Can't rename file."}
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
FTPReply FTPClientState::APPE(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::REST(const std::string &) { return FTPReply::R500; };

FTPReply FTPClientState::PORT(const std::string &) { return FTPReply::R500; };
FTPReply FTPClientState::PASV() { return FTPReply::R500; };
