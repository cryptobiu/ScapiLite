package com.inellipse.biumatrix.service.impl;

import com.inellipse.biumatrix.model.OfflineInstance;
import com.inellipse.biumatrix.model.Poll;
import com.inellipse.biumatrix.service.MatrixService;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;

@Service
public class MatrixServiceImpl implements MatrixService {

    public static final String POLL_TYPE_ONLINE = "online_mobile";
    public static final String POLL_TYPE_OFFLINE = "offline";

    @Value("${matrix.api.url}")
    private String matrixApiUrl;

    @Override
    public void openForRegistration(String pollName) {
        String resourceUrl = String.format("openForRegistration/%s", pollName);
        ResponseEntity<String> response = consumeApi(resourceUrl);
        System.out.println(response.getStatusCode());
    }

    @Override
    public void registerToPoll(Poll poll, OfflineInstance offlineInstance) {
        String resourceUrl = String.format("registerToPoll/%s/%s/%s", poll.getName(), offlineInstance.getIp(), POLL_TYPE_OFFLINE);
        ResponseEntity<String> response = consumeApi(resourceUrl);
        System.out.println(response.getStatusCode());
    }

    @Override
    public void closePollForRegistration() {
        String resourceUrl = "closePollForRegistration";
        ResponseEntity<String> response = consumeApi(resourceUrl);
        System.out.println(response.getStatusCode());
    }

    private ResponseEntity<String> consumeApi(String resourceUrl) {
        RestTemplate restTemplate = new RestTemplate();
        String url = String.format("%s/%s", matrixApiUrl, resourceUrl);

        return restTemplate.getForEntity(url, String.class);
    }
}
