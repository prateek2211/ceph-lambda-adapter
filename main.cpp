#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/lambda/LambdaClient.h>
#include <aws/lambda/model/InvokeRequest.h>
#include <iostream>
#include "httplib.h"

using namespace httplib;

static std::shared_ptr<Aws::Lambda::LambdaClient> client;

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
        Aws::IOStream &pl = result.GetPayload();
        Aws::String returnedData;
        std::getline(pl, returnedData);
        std::cout << returnedData << "\n\n";
    }
}

int main() {
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    Aws::Client::ClientConfiguration configuration;
    client = Aws::MakeShared<Aws::Lambda::LambdaClient>("TEST", configuration);
    Server s;
    s.Post("/", [](const Request &req, Response &res, const ContentReader &content_reader) {
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
            body.append(data, data_length);
            return true;
        });
        res.set_content("Invoking..", "text");
        invoke_lambda("gsoc20", body);
    });
    s.listen("localhost", 8080);
}
