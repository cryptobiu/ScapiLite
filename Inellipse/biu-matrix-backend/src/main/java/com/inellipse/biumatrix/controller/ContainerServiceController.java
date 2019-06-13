package com.inellipse.biumatrix.controller;

import com.inellipse.biumatrix.dto.container.ContainerInfo;
import com.inellipse.biumatrix.service.OfflineInstanceService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
@RequestMapping("cs")
public class ContainerServiceController {

    private OfflineInstanceService offlineInstanceService;

    @Autowired
    public ContainerServiceController(OfflineInstanceService offlineInstanceService) {
        this.offlineInstanceService = offlineInstanceService;
    }

    @PostMapping
    public void updateContainerDetails(@RequestBody ContainerInfo info) {
        offlineInstanceService.updateContainerDetails(info);
    }

    @PostMapping(value = "start")
    public void startTestContainer(@RequestBody Map<String, String> data) {
        offlineInstanceService.startTestContainer(data);
    }

}
