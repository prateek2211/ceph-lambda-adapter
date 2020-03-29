#include <boost/lockfree/queue.hpp>
#include <string>
#include <utility>


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

//    Works on messages in the queque
        void run() {

        }

    public:
        int publish(const std::string &message, const std::string &topic) {
            messageQueue.push(new message_t(message, topic));
        }
    };

    static Manager *manager;

    int publish(const std::string &message, const std::string &topic) {
        return manager->publish(message, topic);
    }

}

