#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/lambda/LambdaClient.h>
#include <aws/lambda/model/InvokeRequest.h>
#include <iostream>
#include "httplib.h"

using namespace httplib;

static std::shared_ptr<Aws::Lambda::LambdaClient> client;
std::string lambda_arn = "";

void invoke_lambda(std::string functionName, std::string payload) {
    Aws::Lambda::Model::InvokeRequest invokeRequest;
    invokeRequest.SetInvocationType(Aws::Lambda::Model::InvocationType::RequestResponse);
    invokeRequest.SetLogType(Aws::Lambda::Model::LogType::Tail);
    invokeRequest.SetFunctionName(functionName);
    std::shared_ptr<Aws::IOStream> body = Aws::MakeShared<Aws::StringStream>(payload.c_str());
    invokeRequest.SetBody(body);
//    invokeRequest.SetContentType();
//    invokeRequest.GetBody()
    auto response = client->Invoke(invokeRequest);

    if (response.IsSuccess()) {
        Aws::Lambda::Model::InvokeResult &result = response.GetResult();

        Aws::IOStream &payload = result.GetPayload();
        Aws::String returnedData;
        std::getline(payload, returnedData);
        std::cout << returnedData << "\n\n";
    }
}

int main() {
    Aws::Client::ClientConfiguration configuration;
    configuration.region = Aws::String("us-east-1");
    client = Aws::MakeShared<Aws::Lambda::LambdaClient>("", configuration);
    Server s;
    s.Post("/", [](const Request &req, Response &res, const ContentReader &content_reader) {
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
            body.append(data, data_length);
            return true;
        });
        invoke_lambda("gsoc20", body);
        std::cout << body;
    });
    s.listen("localhost", 8080);
}
