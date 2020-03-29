//
// Created by prateek on 29/03/20.
//

#ifndef CEPH_LAMBDA_ADAPTER_SNS_H
#define CEPH_LAMBDA_ADAPTER_SNS_H

#endif //CEPH_LAMBDA_ADAPTER_SNS_H

#include <string>

namespace sns {
    void publish_message(std::string message, std::string arn);
};