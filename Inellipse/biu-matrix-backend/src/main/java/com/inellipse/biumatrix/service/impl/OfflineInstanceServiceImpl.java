package com.inellipse.biumatrix.service.impl;

import com.inellipse.biumatrix.dto.OfflineInstanceDTO;
import com.inellipse.biumatrix.dto.container.ContainerConfig;
import com.inellipse.biumatrix.dto.container.ContainerInfo;
import com.inellipse.biumatrix.dto.container.ContainerStartResult;
import com.inellipse.biumatrix.exception.BadRequestException;
import com.inellipse.biumatrix.exception.RecordNotFoundException;
import com.inellipse.biumatrix.model.OfflineInstance;
import com.inellipse.biumatrix.model.Poll;
import com.inellipse.biumatrix.repository.OfflineInstanceRepository;
import com.inellipse.biumatrix.repository.PollRepository;
import com.inellipse.biumatrix.service.AuthService;
import com.inellipse.biumatrix.service.MatrixService;
import com.inellipse.biumatrix.service.OfflineInstanceService;
import com.inellipse.biumatrix.service.container.ContainerService;
import com.inellipse.biumatrix.utils.HttpUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

import javax.servlet.http.HttpServletRequest;
import java.util.Calendar;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@Service
public class OfflineInstanceServiceImpl implements OfflineInstanceService {

    private final ContainerService containerService;
    private final AuthService authService;
    private final OfflineInstanceRepository offlineInstanceRepository;
    private final PollRepository pollRepository;
    private final HttpServletRequest request;
    private final MatrixService matrixService;

    @Autowired
    public OfflineInstanceServiceImpl(ContainerService containerService, AuthService authService,
                                      OfflineInstanceRepository offlineInstanceRepository, PollRepository pollRepository,
                                      HttpServletRequest request, MatrixService matrixService) {
        this.containerService = containerService;
        this.authService = authService;
        this.offlineInstanceRepository = offlineInstanceRepository;
        this.pollRepository = pollRepository;
        this.request = request;
        this.matrixService = matrixService;
    }

    @Override
    public OfflineInstanceDTO createOfflineInstance(Map<String, String> data) {
        String userId = authService.getAuthenticatedUserId();
        String pollId = data.get("pollId");
        String publicKey = data.get("publicKey");

        if (pollId == null) {
            throw new BadRequestException("pollId is required");
        }

        if (publicKey == null) {
            throw new BadRequestException("publicKey is required");
        }

        OfflineInstance offlineInstance = new OfflineInstance();
        offlineInstance.setUserId(userId);
        offlineInstance.setPollId(pollId);
        offlineInstance.setPublicKey(publicKey);
        offlineInstance.setAnswer(false);

        ContainerConfig containerConfig = new ContainerConfig()
                .setUserId(userId)
                .setUserPublicKey(publicKey);

        ContainerStartResult containerStartResult = containerService.startContainer(containerConfig);

        if (containerStartResult.isSuccess()) {
            offlineInstance.setExternalId(containerStartResult.getId());
            offlineInstance.setCid(containerStartResult.getCid());
            // TODO: set ip, port, encryptedEcryptionKey
        }

        offlineInstance = offlineInstanceRepository.save(offlineInstance);
        return new OfflineInstanceDTO(offlineInstance);
    }

    @Override
    public OfflineInstanceDTO changeOfflineInstance(Map<String, Object> data) {

        String cId = (String) data.get("cId");
        Boolean answer = (Boolean) data.get("answer");

        if (cId == null) {
            throw new BadRequestException("cId is required");
        }

        if (answer == null) {
            throw new BadRequestException("answer is required");
        }

        OfflineInstance offlineInstance = offlineInstanceRepository.findByCid(cId);
        if (offlineInstance == null) {
            throw new RecordNotFoundException("Offline instance not found.");
        }

        offlineInstance.setAnswer(answer);
        offlineInstance = offlineInstanceRepository.save(offlineInstance);

        Poll poll = pollRepository.findOne(offlineInstance.getPollId());

        matrixService.registerToPoll(poll, offlineInstance);

        return new OfflineInstanceDTO(offlineInstance);
    }

    @Override
    public OfflineInstanceDTO getOfflineInstance(String id) {
        OfflineInstance offlineInstance = offlineInstanceRepository.findOne(id);
        if (offlineInstance == null) {
            throw new RecordNotFoundException("Offline instance not found.");
        }
        return new OfflineInstanceDTO(offlineInstance);
    }

    @Override
    @Scheduled(cron = "*/1 * * * *")
    public List<OfflineInstanceDTO> sendToMatrix() {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);

        List<String> pollIds = pollRepository.findByExecutionTimeEquals(calendar.getTimeInMillis()).stream()
                .map(Poll::getId)
                .collect(Collectors.toList());

        List<OfflineInstanceDTO> offlineInstances = offlineInstanceRepository.findByPollIdInAndAnswer(pollIds, true)
                .stream().map(OfflineInstanceDTO::new).collect(Collectors.toList());

        // TODO: send info to all containers to start with execution

        return offlineInstances;
    }

    @Override
    public void updateContainerDetails(ContainerInfo info) {
        OfflineInstance offlineInstance = offlineInstanceRepository.findByCid(info.getCid());
        if (offlineInstance == null) {
            throw new RecordNotFoundException("Offline instance not found.");
        }
        offlineInstance.setEncryptedEcryptionKey(info.getEncKey());
        offlineInstance.setIp(HttpUtils.getIpAddress(request));
        offlineInstance.setPort("8080");
        offlineInstanceRepository.save(offlineInstance);
    }

    @Override
    public void startTestContainer(Map<String, String> data) {
        OfflineInstance offlineInstance = new OfflineInstance();
        offlineInstance.setUserId(data.get("userId"));
        offlineInstance.setPollId(data.get("pollId"));
        offlineInstance.setPublicKey(data.get("publicKey"));
        offlineInstance.setAnswer(false);

        ContainerConfig containerConfig = new ContainerConfig()
                .setUserId(data.get("userId"))
                .setUserPublicKey(data.get("publicKey"));

        ContainerStartResult containerStartResult = containerService.startContainer(containerConfig);
        if (containerStartResult.isSuccess()) {
            offlineInstance.setExternalId(containerStartResult.getId());
            offlineInstance.setCid(containerStartResult.getCid());
            // TODO: set ip, port, encryptedEcryptionKey
        }

        offlineInstanceRepository.save(offlineInstance);
    }
}
