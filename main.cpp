#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/lambda/LambdaClient.h>
#include <aws/lambda/model/InvokeRequest.h>
#include <iostream>
#include "httplib.h"
#include <aws/core/utils/json/JsonSerializer.h>

using namespace httplib;

static std::shared_ptr<Aws::Lambda::LambdaClient> client;

const char *ALLOCATION_TAG = "TEST";

void invoke_lambda(const std::string &functionName, const std::string &payload, Response &httpResp) {
    Aws::Lambda::Model::InvokeRequest invokeRequest;
    invokeRequest.SetInvocationType(Aws::Lambda::Model::InvocationType::RequestResponse);
    invokeRequest.SetLogType(Aws::Lambda::Model::LogType::Tail);
    invokeRequest.SetFunctionName(functionName);
    std::shared_ptr<Aws::IOStream> body = Aws::MakeShared<Aws::StringStream>(ALLOCATION_TAG);
    *body << payload;
    invokeRequest.SetBody(body);
    invokeRequest.SetContentType("application/json");
//    Aws::String b;
//    std::getline(*invokeRequest.GetBody(), b);
//    std::cout << b;
    auto response = client->Invoke(invokeRequest);
    if (!response.IsSuccess()) {
        std::cout << response.GetError().GetMessage() << std::endl;
        httpResp.set_content("Failed: Could not invoke", "text/plain");
    } else {
        Aws::Lambda::Model::InvokeResult &result = response.GetResult();
        Aws::IOStream &pl = result.GetPayload();
        Aws::String returnedData;
        std::getline(pl, returnedData);
        std::cout << "Success " << returnedData << "\n" << std::endl;
        httpResp.set_content(returnedData, "application/json");
    }
}

int main(int argc, char **argv) {
    Aws::SDKOptions options = Aws::SDKOptions();
    Aws::InitAPI(options);
    Aws::Client::ClientConfiguration configuration;
    configuration.verifySSL = true;
    configuration.connectTimeoutMs = 30000;
    client = Aws::MakeShared<Aws::Lambda::LambdaClient>(ALLOCATION_TAG, configuration);
    Server s;
    s.Post("/", [](const Request &req, Response &res, const ContentReader &content_reader) {
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
            body.append(data, data_length);
            return true;
        });
        Aws::Utils::Json::JsonValue jsonValue(body);
        jsonValue.AsObject(jsonValue);
        invoke_lambda("gsoc20", body, res);
    });
    s.listen("localhost", 8080);
}
