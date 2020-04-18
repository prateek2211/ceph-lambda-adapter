#include <aws/core/Aws.h>
#include <aws/lambda/LambdaClient.h>
#include <aws/lambda/model/InvokeRequest.h>
#include "httplib.h"
#include "aws/sns.h"
#include "aws/lambda.h"

#define MODE_LAMBDA 1
#define MODE_SNS 2
using namespace httplib;

static int MODE;

bool parse_flags(std::string &mode, int &port, int argc, char **argv) {
    for (int i = 1; i < argc - 1; ++i) {
        if (strcmp(argv[i], "-m") == 0) {
            mode = argv[i + 1];
        } else if (strcmp(argv[i], "-p") == 0)
            port = atoi(argv[i + 1]);
    }
    return !(!port || mode.empty());
}

int main(int argc, char **argv) {
    Aws::SDKOptions options = Aws::SDKOptions();
    Aws::InitAPI(options);
    Server s;
    int port;
    std::string mode;
    if (!parse_flags(mode, port, argc, argv)) {
        std::cout << "provide mode and port\n";
        return EXIT_FAILURE;
    }
    if (mode == "lambda") {
        lambda::init();
        MODE = MODE_LAMBDA;
    } else if (mode == "sns") {
        sns::init();
        MODE = MODE_SNS;
    } else {
        std::cout << mode << " mode not defined\nExiting...";
        return EXIT_FAILURE;
    }

    s.Post("/", [](const Request &req, Response &res, const ContentReader &content_reader) {
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
            body.append(data, data_length);
            return true;
        });
        MODE == MODE_SNS ? sns::publish(body, "arn:aws:sns:us-east-1:125341253834:gsoc20-ceph") : lambda::publish(body,
                                                                                                                  "gsoc20");
        std::cout << "Added message to queue" << std::endl;

    });
    if (!s.listen("localhost", port)) {
        std::cout << "bad port provided\nExiting...";
        return EXIT_FAILURE;
    }

}
