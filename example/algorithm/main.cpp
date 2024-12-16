#include "Vidu_SDK_Api.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>

// algo operat node name
#define ALGO_NUM_NODE_NAME    "AlgoYamlFileNum"
#define ALGO_SELECT_NODE_NAME "AlgoYamlFileSelect"
#define ALGO_CMD_NODE_NAME    "AlgoCmd"
#define ALGO_NAME_NODE_NAME   "AlgoYamlFileName"
#define ALGO_YAML_NODE_NAME   "AlgoYamlFileData"

// demo algorithm result head
typedef struct
{
    uint32_t hash_resev_;
    uint16_t frame_id_;
    uint8_t task_id_;
    uint8_t graph_num_;
} metadata_head_t;

int main()
{
    PDdevice dev_inst;
    if (!dev_inst.init())
    {
        printf("no valid device\n");
        return -1;
    }

    // Get algorithm num of device
    int32_t algo_num = 0;
    if (!dev_inst.get(ALGO_NUM_NODE_NAME, algo_num))
    {
        printf("device instance has not support algorithm\n");
        return -1;
    }

    if (!algo_num)
    {
        printf("device instance has not algorithm yet\n");
        return -1;
    }
    printf("algorithm num: %d\n", algo_num);

    // list algorithm
    for (int32_t index = 0; index < algo_num; ++index)
    {
        // set algorithm index, indicate which algorithm to operate
        if (!dev_inst.set(ALGO_SELECT_NODE_NAME, index))
        {
            printf("set algorithm index %d failed\n", index);
            continue;
        }

        // get algorithm status, 0-off, 1-on, other-error
        int32_t state = 0;
        if (!dev_inst.get(ALGO_CMD_NODE_NAME, state))
        {
            printf("get algorithm index %d state failed\n", index);
        }

        // get algorithm name
        char algo_name[32 + 1] = {0};
        if (!dev_inst.get(ALGO_NAME_NODE_NAME, algo_name, 32))
        {
            printf("get algorithm index %d name failed\n", index);
        }
        else
        {
            printf("algo index %d name: %s, state: %d\n", index, algo_name, state);
        }
    }

    // run algorithm, for example index 3
    int32_t index = 3;
    dev_inst.set(ALGO_SELECT_NODE_NAME, index);
    int32_t state = 0;
    dev_inst.get(ALGO_CMD_NODE_NAME, state);
    if (state == 0)
    {
        char algo_name[32 + 1] = {0};
        dev_inst.get(ALGO_NAME_NODE_NAME, algo_name, 32);

        // get the configuration file that describes the algorithm results
        char yaml_data[10240 + 1] = {0};
        if (!dev_inst.get(ALGO_YAML_NODE_NAME, yaml_data, 10240))
        {
            printf("get algorithm index %d name %s yaml data failed\n", index, algo_name);
            return -1;
        }
        else
        {
            printf("algorithm index %d name %s yaml data:\n%s\n", index, algo_name, yaml_data);
        }

        // enable algorithm
        if (!dev_inst.set(ALGO_CMD_NODE_NAME, 1))
        {
            printf("open algorithm index %d name %s failed\n", index, algo_name);
            return -1;
        }
        else
        {
            printf("open algorithm index %d name %s success\n", index, algo_name);
        }
    }

    auto stream = PDstream(dev_inst, "RGB");
    if (!stream.init())
    {
        printf("stream init failed\n");
        return -1;
    }

    stream.set("AutoExposure", true);
    stream.set("StreamFps", 30);

    while (1)
    {
        // Obtain RGB frame data
        auto frame = stream.waitFrames();
        int32_t key = 0;
        if (frame)
        {
            // Obtain image data from frame data
            const PDimage &rgb = frame->getImage(0);
            cv::Mat rgb_mat =
                cv::Mat(cv::Size(rgb.GetImageWidth(), rgb.GetImageHeight()), rgb.GetImageCVType(), rgb.GetImageData());
            cv::imshow("rgb", rgb_mat);
            key = cv::waitKey(1);

            // Obtain algorithm results through metadata
            char algo_result[28800 + 1] = {0};
            size_t size = sizeof(algo_result);
            if (GenTL::PDBufferGetMetaByName(frame->getPort(), "AlgoResults", nullptr, algo_result, &size) !=
                GenTL::GC_ERR_SUCCESS)
            {
                printf("get algo results failed\n");
            }
            else
            {
                metadata_head_t head = *(metadata_head_t *)algo_result;
                if (head.graph_num_ != 0)
                {
                    printf("algo demo result head: hash: %d, frame id: %d, task id: %d, graph num: %d\n",
                           head.hash_resev_, head.frame_id_, head.task_id_, head.graph_num_);
                }
            }

            // Obtain algorithm results through interface
            GenTL::INFO_DATATYPE type;
            size = sizeof(algo_result);
            if (GenTL::PDBufferGetInfo(frame->getPort(), GenTL::BUFFER_INFO_ALGO_RESULTS, &type, algo_result, &size) ==
                GenTL::GC_ERR_SUCCESS)
            {
                if (type != GenTL::INFO_DATATYPE_STRING)
                {
                    continue;
                }
                metadata_head_t head = *(metadata_head_t *)algo_result;
                if (head.graph_num_ != 0)
                {
                    printf("algo demo result head: hash: %d, frame id: %d, task id: %d, graph num: %d\n",
                           head.hash_resev_, head.frame_id_, head.task_id_, head.graph_num_);
                }
            }
        }

        if (key == 'q')
        {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // stop algorithm
    index = 3;
    dev_inst.set(ALGO_SELECT_NODE_NAME, index);
    state = 0;
    dev_inst.get(ALGO_CMD_NODE_NAME, state);
    if (state == 1)
    {
        char algo_name[32 + 1] = {0};
        dev_inst.get(ALGO_NAME_NODE_NAME, algo_name, 32);

        // disable algorithm
        if (!dev_inst.set(ALGO_CMD_NODE_NAME, 0))
        {
            printf("close algorithm index %d name %s failed\n", index, algo_name);
            return -1;
        }
    }

    return 0;
}
