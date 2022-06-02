#include "pch.h"
#include "reversi_net.h"

int add(int a, int b) {
	return a + b;
}

int parse_response(requests::Response res, char* message, size_t message_limit) {
	std::string msg;
	if (res == nullptr) {
		msg = "Unknown Error";
		if (message)
			strncpy(message, msg.c_str(), message_limit);
		return -1;
	} else {
		msg = res->Body();
		if (message)
			strncpy(message, msg.c_str(), message_limit);
		return res->status_code;
	}
}

int new_game(int side, char* message, size_t message_limit) {
	hv::Json j;
	j["side"] = side;

	http_headers headers;
	headers["Content-Type"] = "application/json";
	auto res = requests::post((base + "/new").c_str(), j.dump(), headers);
	return parse_response(res, message, message_limit);
}

int terminate_game(char* message, size_t message_limit) {
	auto res = requests::post((base + "/end").c_str());
	std::string msg;
	return parse_response(res, message, message_limit);
}

int place_chess(int index, char* message, size_t message_limit) {
	hv::Json j;
	j["pos"] = index;
	http_headers headers;
	headers["Content-Type"] = "application/json";
	auto res = requests::post((base + "/place").c_str(), j.dump(), headers);
	return parse_response(res, message, message_limit);
}

int get_board(int* board, int* next_player, bool* over, int* winner, char* message, size_t message_limit) {
	auto res = requests::get((base + "/board").c_str());
	std::string msg;
	if (res == nullptr) {
		msg = "Unknown Error";
		if (message)
			strncpy(message, msg.c_str(), message_limit);
		return -1;
	}
	else if (res->status_code != 200) {
		msg = res->Body();
		if (message)
			strncpy(message, msg.c_str(), message_limit);
		return res->status_code;
	}

	hv::Json j;
	int ret = hv::parse_json(res->Body().c_str(), j, msg);
	if (ret != 0) {
		if (message)
			strncpy(message, msg.c_str(), message_limit);
		return ret;
	}
	
	auto new_board = j["board"].get<std::array<int, 64>>();
	for (int i = 0; i < 64; ++i) {
		board[i] = new_board[i];
	}

	if (next_player)
		*next_player = j["next_player"].get<int>();

	if (over)
		*over = j["over"].get<bool>();

	if (winner)
		*winner = j["winner"].get<int>();

	if (message)
		strncpy(message, res->status_message(), message_limit);

	return res->status_code;
}