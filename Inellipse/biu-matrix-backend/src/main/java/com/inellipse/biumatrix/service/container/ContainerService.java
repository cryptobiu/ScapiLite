package com.inellipse.biumatrix.service.container;

import com.inellipse.biumatrix.dto.container.ContainerConfig;
import com.inellipse.biumatrix.dto.container.ContainerStartResult;
import com.inellipse.biumatrix.dto.container.ContainerStopResult;

public interface ContainerService {
    ContainerStartResult startContainer(ContainerConfig config);
    ContainerStopResult restartContainer(String containerId);
    ContainerStopResult stopContainer(String containerId);
}
