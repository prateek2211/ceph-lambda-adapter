#include <boost/lockfree/queue.hpp>
#include <string>
#include <utility>
#include <aws/sns/SNSClient.h>
#include <aws/sns/model/PublishRequest.h>

namespace sns {
    static const int STATUS_OK = 1;

    struct message_t {
        std::string topic;
        std::string message;

        message_t(std::string topic, std::string message) : topic(std::move(topic)), message(std::move(message)) {};
    };

    typedef boost::lockfree::queue<message_t *, boost::lockfree::fixed_sized<true>> MessageQueue;

    class Manager {
    private:
        MessageQueue messageQueue;
        std::thread runner;

        void publish_internal(message_t *msg) {
            std::cout << "Publishing.." << std::endl;
            Aws::SNS::Model::PublishRequest req;
            req.SetMessage(msg->message);
            req.SetTopicArn(msg->topic);
            Aws::SNS::SNSClient client;

            auto result = client.Publish(req);
            if (result.IsSuccess()) {
                std::cout << "Published..." << std::endl;
            } else {
                std::cout << result.GetError().GetMessage() << std::endl;
            }
        }

//    Works on messages in the queque
        void run() {
            while (true) {
                const auto count = messageQueue.consume_all(
                        std::bind(&Manager::publish_internal, this, std::placeholders::_1));
            }
        }

    public:
        Manager() : messageQueue(10), runner(&Manager::run, this) {}

        int publish(const std::string &message, const std::string &topic) {
            messageQueue.push(new message_t(topic, message));
            return STATUS_OK;
        }
    };

    static Manager *manager;

    void init() {
        manager = new Manager();
    }

    int publish(const std::string &message, const std::string &topic) {
        return manager->publish(message, topic);
    }
}

