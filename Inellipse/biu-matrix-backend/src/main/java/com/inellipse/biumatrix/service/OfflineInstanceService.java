package com.inellipse.biumatrix.service;

import com.inellipse.biumatrix.dto.OfflineInstanceDTO;
import com.inellipse.biumatrix.dto.container.ContainerInfo;

import java.util.List;
import java.util.Map;

public interface OfflineInstanceService {
    OfflineInstanceDTO createOfflineInstance(Map<String, String> data);
    OfflineInstanceDTO changeOfflineInstance(Map<String, Object> data);
    OfflineInstanceDTO getOfflineInstance(String id);
    List<OfflineInstanceDTO> sendToMatrix();

    void updateContainerDetails(ContainerInfo info);
    void startTestContainer(Map<String, String> data);
}
