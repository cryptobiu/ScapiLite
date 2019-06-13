package com.inellipse.biumatrix.service.container;

import com.inellipse.biumatrix.dto.container.ContainerConfig;
import com.inellipse.biumatrix.dto.container.ContainerStartResult;
import com.inellipse.biumatrix.dto.container.ContainerStopResult;
import io.iron.ironworker.client.Client;
import io.iron.ironworker.client.entities.TaskEntity;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.util.Collections;

@Service
public class IronIoContainerService implements ContainerService {

    private final Logger logger = LoggerFactory.getLogger(getClass());

    @Value("${cs.provider.iron.io.credentials.token}")
    private String token;

    @Value("${cs.provider.iron.io.credentials.project}")
    private String project;

    @Value("${cs.provider.iron.io.container.image}")
    private String image;

    private Client workerClient = null;

    @Override
    public ContainerStartResult startContainer(ContainerConfig config) {
        TaskEntity result = null;
        try {
            result = getWorkerClient().createTask(image, Collections.singletonMap("config", config));
        } catch (Exception e) {
            logger.error("unable to start worker: ", e);
        }
        return getContainerStartResult(result);
    }

    @Override
    public ContainerStopResult restartContainer(String containerId) {
        try {
            TaskEntity taskEntity = getWorkerClient().getTask(containerId);
            if (taskEntity != null) {
                // TODO:
            }
        } catch (Exception e) {

        }
        return null;
    }

    @Override
    public ContainerStopResult stopContainer(String containerId) {
        boolean result = false;
        try {
            result = getWorkerClient().cancelTask(containerId);
        } catch (Exception e) {
            logger.error("unable to stop running container: " + containerId, e);
        }
        return getContainerStopResult(containerId, result);
    }

    private Client getWorkerClient() {
        if (workerClient == null) {
            workerClient = new Client(token, project);
        }
        return workerClient;
    }

    private ContainerStartResult getContainerStartResult(TaskEntity taskEntity) {
        if (taskEntity == null) {
            return null;
        }
        return null;
//        return new ContainerStartResult(taskEntity);
    }

    private ContainerStopResult getContainerStopResult(String containerId, boolean success) {
        return new ContainerStopResult(containerId, success);
    }
}
