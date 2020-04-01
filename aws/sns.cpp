#include <boost/lockfree/queue.hpp>
#include <string>
#include <utility>
#include <aws/sns/SNSClient.h>
#include <aws/sns/model/PublishRequest.h>

namespace sns {

    static const int STATUS_OK = 1;
    static const int STATUS_FULL = 2;
    static const int STATUS_MANAGER_STOPPED = 3;

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
        bool stopped;

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
            while (!stopped) {
                const auto count = messageQueue.consume_all(
                        std::bind(&Manager::publish_internal, this, std::placeholders::_1));
            }
        }

    public:
        Manager(size_t max_queue_size) : messageQueue(max_queue_size), runner(&Manager::run, this), stopped(false) {}

        int publish(const std::string &message, const std::string &topic) {
            if (stopped)
                return STATUS_MANAGER_STOPPED;
            if (messageQueue.push(new message_t(topic, message)))
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

    int publish(const std::string &message, const std::string &topic) {
        if (!manager)
            return STATUS_MANAGER_STOPPED;
        return manager->publish(message, topic);
    }
}

