//
// Created by prateek on 30/03/20.
//

#ifndef CEPH_LAMBDA_ADAPTER_LAMBDA_H
#define CEPH_LAMBDA_ADAPTER_LAMBDA_H

#endif //CEPH_LAMBDA_ADAPTER_LAMBDA_H

#include <string>
#include "../httplib.h"

namespace lambda {
    void invoke_lambda(const std::string &functionName, const std::string &payload, httplib::Response &httpResp);
}