package com.inellipse.biumatrix;

import com.inellipse.biumatrix.dto.container.ContainerConfig;
import com.inellipse.biumatrix.service.container.ContainerService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.stereotype.Component;

@Component
public class EventListener implements CommandLineRunner {

    private final ContainerService containerService;

    @Autowired
    public EventListener(ContainerService containerService) {
        this.containerService = containerService;
    }

    @Override
    public void run(String... strings) throws Exception {
        ContainerConfig config = new ContainerConfig()
                .setUserId("testuserId")
                .setUserPublicKey("-----BEGIN RSA PUBLIC KEY-----\n" +
                        "MIGJAoGBALdnlEH/n6YSDetq7rjZsWOY/zTNlHRBQWy/cgkTCojMRmL/NtDTRUG5\n" +
                        "5BygDyyMDbRG9hbxW4Uk1gCavOfmhsNVnFpJB8Kt1L6Oy//KDpVDLckqb1xnMDqI\n" +
                        "8fKtY9htF3S1DTEAdzcoG2yuhK3E9kK14xr0lQmmXy6zBO3ucD6JAgMBAAE=\n" +
                        "-----END RSA PUBLIC KEY-----");
//        containerService.startContainer(config);
    }
}
