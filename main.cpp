#include <aws/core/Aws.h>
#include <aws/lambda/LambdaClient.h>
#include <aws/lambda/model/InvokeRequest.h>
#include "httplib.h"
#include "aws/sns.h"
#include "aws/lambda.h"

using namespace httplib;

int main(int argc, char **argv) {
    Aws::SDKOptions options = Aws::SDKOptions();
    Aws::InitAPI(options);
    Server s;
    s.Post("/", [](const Request &req, Response &res, const ContentReader &content_reader) {
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
            body.append(data, data_length);
            return true;
        });
//        sns::publish_message(body, "arn:aws:sns:us-east-1:125341253834:gsoc20-ceph");
        lambda::invoke_lambda("gsoc20", body, res);
    });
    s.listen("localhost", 8080);
}
