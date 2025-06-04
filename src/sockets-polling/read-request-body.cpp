#include "../../include/sockets-polling.h"

// int readRequestBody(int fd, std::string& bodyStr, int contentLength)
// {
// 	(void)contentLength;
//
//
//     char buf[BODY_BUFFER_SIZE + 1]; 
//
//     while (true) {
//         int nBytes = recv(fd, buf, BODY_BUFFER_SIZE, 0);
//         if (nBytes == -1) { // Error receiving data
// 			std::cout << "recv: no data available in socket\n";
//
//             return -1;
//         } else if (nBytes == 0) {
//             // Client connection closed
//             std::cout << "recv: socket " << fd << " hung up\n"; 
//
//             return -2;
//         }
//         buf[nBytes] = '\0'; // Null-terminate the received data
//         bodyStr += buf;  // Append to the string
//     }
//
// 	std::cout << "\n===== Body String: =====\n";
// 	std::cout << bodyStr << '\n';
// 	std::cout << "==========================\n\n";
//
//     const std::string response = "Response from server\n";
//     send(fd, response.c_str(), response.size(), 0);
// }

// TODO: handle `contentLength` checking
// int readBodyToBuffers(int fd, std::vector<Buffer>& buffers) {
//     if (buffers.empty())
//         buffers.push_back(Buffer(BODY_BUFFER_SIZE));
//
//     while (1) {
//         Buffer& current = buffers.back();
//
//         int ret = readFromSocket(fd, current);
//         if (ret == NGX_AGAIN)
//             return NGX_AGAIN;
//         if (ret == NGX_ERROR)
//             return NGX_ERROR;
// 			//      if (hasEndOfHeaderSection(current))
// 			// return NGX_OK;
//
//         if (current.remainingSize() == 0) 
//             buffers.push_back(Buffer(BODY_BUFFER_SIZE));
//     }
//
// }

void printBuffers(std::vector<Buffer>& buffers)
{
	std::cout << "\nbuffers: \n";
	std::vector<Buffer>::iterator it = buffers.begin();
	for (; it != buffers.end(); ++it) {
		std::string str(it->data.begin(), it->data.end());
		std::cout << str << '\n';
	}


}

void combineBuffersToSTring(std::vector<Buffer>& buffers, std::string& bodyStr)
{

	std::cout << "Size of buffers: " << buffers.size() << '\n';
	std::vector<Buffer>::iterator it = buffers.begin();
	for (; it != buffers.end(); ++it) {
		// std::string bufStr(it->data.begin(), it->data.end());
		// std::cout << "bufStr.length: " << bufStr.length() << '\n';
		// bodyStr += bufStr;
		bodyStr.append(it->data.begin(), it->data.end());
		std::cout << "Size of data: " << it->data.end() - it->data.begin() << '\n';

	
		
		std::vector<char>::iterator bufIt = it->data.begin();
		for (; bufIt != it->data.end(); ++bufIt) {
			std::cout << *bufIt;
			// std::cout << "check\n" << '\n';
		}

		std::cout << '\n';
	}
}

void readRequestBody(int fd, std::string& bodyStr, std::vector<Buffer>& buffers, int contentLength)
{
	std::cout << "\nREAD REQUEST BODY\n";
	(void)contentLength;
	(void)fd;
	// TODO: Handle return values/errors 
	// readBodyToBuffers(fd, buffers);
	
	// printBuffers(buffers);

	combineBuffersToSTring(buffers, bodyStr);
	std::cout << "\n===== Body String: =====\n";
	std::cout << "|" << bodyStr << "|" << '\n';
	std::cout << "==========================\n\n";
	std::cout << "Body size: " << bodyStr.length() << '\n';

}

