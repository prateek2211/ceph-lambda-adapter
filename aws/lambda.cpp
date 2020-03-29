//
// Created by prateek on 30/03/20.
//

#include <string>
#include <iostream>
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/lambda/LambdaClient.h>
#include <aws/lambda/model/InvokeRequest.h>
#include "../httplib.h"

namespace lambda {
    const char *ALLOCATION_TAG = "TEST";
    static std::shared_ptr<Aws::Lambda::LambdaClient> client;

    void invoke_lambda(const std::string &functionName, const std::string &payload, httplib::Response &httpResp) {
        Aws::Client::ClientConfiguration configuration;
        configuration.verifySSL = true;
        configuration.connectTimeoutMs = 30000;
        client = Aws::MakeShared<Aws::Lambda::LambdaClient>(ALLOCATION_TAG, configuration);

        Aws::Lambda::Model::InvokeRequest invokeRequest;
        invokeRequest.SetInvocationType(Aws::Lambda::Model::InvocationType::RequestResponse);
        invokeRequest.SetLogType(Aws::Lambda::Model::LogType::Tail);
        invokeRequest.SetFunctionName(functionName);
        std::shared_ptr<Aws::IOStream> body = Aws::MakeShared<Aws::StringStream>(ALLOCATION_TAG);
        *body << payload;
        invokeRequest.SetBody(body);
        invokeRequest.SetContentType("application/json");
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
}