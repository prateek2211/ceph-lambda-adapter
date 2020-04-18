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

    }

}

#include <boost/lockfree/queue.hpp>
#include <string>
#include <utility>
#include <aws/sns/SNSClient.h>
#include <aws/sns/model/PublishRequest.h>

namespace lambda {

    static const int STATUS_OK = 1;
    static const int STATUS_FULL = 2;
    static const int STATUS_MANAGER_STOPPED = 3;

    struct message_t {
        std::string function_name;
        std::string payload;

        message_t(std::string function_name, std::string payload) : function_name(std::move(function_name)),
                                                                    payload(std::move(payload)) {};
    };

    typedef boost::lockfree::queue<message_t *, boost::lockfree::fixed_sized<true>> MessageQueue;

    class Manager {
    private:
        MessageQueue messageQueue;
        std::thread runner;
        bool stopped;

        void publish_internal(message_t *msg) {
            std::cout << "Invoking.." << std::endl;
            Aws::Client::ClientConfiguration configuration;
            configuration.verifySSL = true;
            configuration.connectTimeoutMs = 30000;
            client = Aws::MakeShared<Aws::Lambda::LambdaClient>(ALLOCATION_TAG, configuration);

            Aws::Lambda::Model::InvokeRequest invokeRequest;
            invokeRequest.SetInvocationType(Aws::Lambda::Model::InvocationType::RequestResponse);
            invokeRequest.SetLogType(Aws::Lambda::Model::LogType::Tail);
            invokeRequest.SetFunctionName(msg->function_name);
            std::shared_ptr<Aws::IOStream> body = Aws::MakeShared<Aws::StringStream>(ALLOCATION_TAG);
            *body << msg->payload;
            invokeRequest.SetBody(body);
            invokeRequest.SetContentType("application/json");
            auto response = client->Invoke(invokeRequest);
            if (!response.IsSuccess()) {
                std::cout << response.GetError().GetMessage() << std::endl;
            } else {
                Aws::Lambda::Model::InvokeResult &result = response.GetResult();
                Aws::IOStream &pl = result.GetPayload();
                Aws::String returnedData;
                std::getline(pl, returnedData);
                std::cout << "Success " << returnedData << "\n" << std::endl;
            }
        }

//    Works on messages in the queque
        void run() {
            while (!stopped) {
                const auto count = messageQueue.consume_all(
                        std::bind(&Manager::publish_internal, this, std::placeholders::_1));
            }
        }

    public:
        Manager(size_t max_queue_size) : messageQueue(max_queue_size), runner(&Manager::run, this), stopped(false) {}

        int publish(const std::string &payload, const std::string &function_name) {
            if (stopped)
                return STATUS_MANAGER_STOPPED;
            if (messageQueue.push(new message_t(function_name, payload)))
                return STATUS_OK;
            return STATUS_FULL;
        }

        void stop() {
            stopped = true;
        }

    };

    static Manager *manager;

    static const size_t MAX_QUEUE_DEFAULT = 8192;

    bool init() {
        if (manager) {
            return false;
        }
        manager = new Manager(MAX_QUEUE_DEFAULT);
        return true;
    }

    // Publishes the message to the queue
    int publish(const std::string &payload, const std::string &function_name) {
        if (!manager)
            return STATUS_MANAGER_STOPPED;
        return manager->publish(payload, function_name);
    }
}

