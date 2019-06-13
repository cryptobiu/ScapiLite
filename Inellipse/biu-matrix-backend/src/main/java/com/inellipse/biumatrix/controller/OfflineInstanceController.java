package com.inellipse.biumatrix.controller;

import com.inellipse.biumatrix.dto.OfflineInstanceDTO;
import com.inellipse.biumatrix.service.OfflineInstanceService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping(value = "offline-instance")
public class OfflineInstanceController {

    private final OfflineInstanceService offlineInstanceService;

    @Autowired
    public OfflineInstanceController(OfflineInstanceService offlineInstanceService) {
        this.offlineInstanceService = offlineInstanceService;
    }

    @PostMapping(value = "create")
    public OfflineInstanceDTO createOfflineInstance(@RequestBody Map<String, String> data) {
        return offlineInstanceService.createOfflineInstance(data);
    }

    @GetMapping(value = "{offlineInstanceId}")
    public OfflineInstanceDTO getOfflineInstance(@PathVariable("offlineInstanceId") String id) {
        return offlineInstanceService.getOfflineInstance(id);
    }

    @PutMapping
    public OfflineInstanceDTO setOfflineInstanceAnswer(@RequestBody Map<String, Object> data) {
        return offlineInstanceService.changeOfflineInstance(data);
    }

}
