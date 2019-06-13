package com.inellipse.biumatrix.service.container;

import com.amazonaws.auth.AWSStaticCredentialsProvider;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.ecs.AmazonECS;
import com.amazonaws.services.ecs.AmazonECSClientBuilder;
import com.amazonaws.services.ecs.model.*;
import com.inellipse.biumatrix.dto.container.ContainerConfig;
import com.inellipse.biumatrix.dto.container.ContainerStartResult;
import com.inellipse.biumatrix.dto.container.ContainerStopResult;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Primary;
import org.springframework.stereotype.Service;

import java.util.UUID;

@Service
@Primary
public class AWSContainerService implements ContainerService {

    private final Logger logger = LoggerFactory.getLogger(getClass());

    private static final String FARGATE = "FARGATE";
    private static final String ASSIGN_PUBLIC_IP = "ENABLED";

    private static final String USER_ID = "USER_ID";
    private static final String PUB_KEY = "PUB_KEY";
    private static final String CID = "CID";

    @Value("${ecs.fargate.api.key}")
    private String apiKey;

    @Value("${ecs.fargate.api.secret}")
    private String apiSecret;

    @Value("${ecs.fargate.subnet}")
    private String subnet;

    @Value("${ecs.fargate.security.group}")
    private String securityGroup;

    @Value("${ecs.fargate.cluster}")
    private String cluster;

    @Value("${ecs.fargate.task.definition}")
    private String taskDefinition;

    @Value("${ecs.fargate.container.name}")
    private String containerName;

    private AmazonECS ecsClient;

    @Override
    public ContainerStartResult startContainer(ContainerConfig config) {

        // generate unique id for container (used in update info method)
        String cid = UUID.randomUUID().toString();

        // setup network configuration
        AwsVpcConfiguration vpcConfiguration = new AwsVpcConfiguration();
        vpcConfiguration.withSubnets(subnet);
        vpcConfiguration.withSecurityGroups(securityGroup);
        vpcConfiguration.setAssignPublicIp(ASSIGN_PUBLIC_IP);

        NetworkConfiguration networkConfiguration = new NetworkConfiguration()
                .withAwsvpcConfiguration(vpcConfiguration);

        // override container environment
        ContainerOverride containerOverride = new ContainerOverride()
                .withName(containerName)
                .withEnvironment(
                        new KeyValuePair().withName(USER_ID).withValue(config.getUserId()),
                        new KeyValuePair().withName(PUB_KEY).withValue(config.getUserPublicKey()),
                        new KeyValuePair().withName(CID).withValue(cid)
                );

        TaskOverride taskOverride = new TaskOverride();
        taskOverride.withContainerOverrides(containerOverride);

        // create request
        RunTaskRequest request = new RunTaskRequest()
                .withCluster(cluster)
                .withTaskDefinition(taskDefinition)
                .withOverrides(taskOverride)
                .withNetworkConfiguration(networkConfiguration)
                .withLaunchType(FARGATE);

        try {
            RunTaskResult result = getECSClient().runTask(request);
            logger.info("start container response: " + result);
            return new ContainerStartResult(result, cid,true);
        } catch (Exception e) {
            logger.error("start container response error: ", e);
            return new ContainerStartResult(false);
        }
    }

    @Override
    public ContainerStopResult restartContainer(String containerId) {
        return null;
    }

    @Override
    public ContainerStopResult stopContainer(String containerId) {
        if (containerId == null || containerId.isEmpty()) {
            return null;
        }

        StopTaskRequest request = new StopTaskRequest()
                .withCluster(cluster)
                .withReason("Stop from Admin interface")
                .withTask(containerId);

        try {
            StopTaskResult result = getECSClient().stopTask(request);
            logger.info("stop container response: " + result);
        } catch (Exception e) {
            logger.error("stop container response error: ", e);
        }
        return null;
    }

    private AmazonECS getECSClient() {
        if (ecsClient == null) {
            ecsClient = AmazonECSClientBuilder.standard()
                    .withCredentials(new AWSStaticCredentialsProvider(new BasicAWSCredentials(apiKey, apiSecret)))
                    .withRegion(Regions.EU_WEST_1).build();
        }
        return ecsClient;
    }

}
