package com.inellipse.biumatrix.service.impl;

import com.inellipse.biumatrix.dto.PollDTO;
import com.inellipse.biumatrix.dto.UserDTO;
import com.inellipse.biumatrix.exception.RecordNotFoundException;
import com.inellipse.biumatrix.model.Poll;
import com.inellipse.biumatrix.model.PollDetails;
import com.inellipse.biumatrix.repository.PollDetailRepository;
import com.inellipse.biumatrix.repository.PollRepository;
import com.inellipse.biumatrix.repository.UserRepository;
import com.inellipse.biumatrix.service.AuthService;
import com.inellipse.biumatrix.service.MatrixService;
import com.inellipse.biumatrix.service.PollService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageImpl;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.aggregation.Aggregation;
import org.springframework.data.mongodb.core.aggregation.AggregationResults;
import org.springframework.data.mongodb.core.query.Criteria;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

import java.util.*;
import java.util.stream.Collectors;

@Service
public class PollServiceImpl implements PollService {

    private PollRepository pollRepository;
    private PollDetailRepository pollDetailRepository;
    private UserRepository userRepository;
    private AuthService authService;
    private MatrixService matrixService;

    @Autowired
    public PollServiceImpl(PollRepository pollRepository, PollDetailRepository pollDetailRepository, UserRepository userRepository, AuthService authService, MatrixService matrixService) {
        this.pollRepository = pollRepository;
        this.pollDetailRepository = pollDetailRepository;
        this.userRepository = userRepository;
        this.authService = authService;
        this.matrixService = matrixService;
    }

    @Override
    public void savePoll(PollDTO pollDTO) {
        Poll poll = new Poll();
        poll.setTitle(pollDTO.getTitle());
        poll.setDescription(pollDTO.getDescription());
        poll.setName(UUID.randomUUID().toString());
        poll.setExecutionTime(pollDTO.getExecutionTime());
        poll.setUserRegistrationSecondsBeforeExecution(pollDTO.getUserRegistrationSecondsBeforeExecution());
        poll.setActive(pollDTO.getActive());
        poll.setResultType(pollDTO.getResultType());

        pollRepository.save(poll);

        matrixService.openForRegistration(poll.getName());
    }

    @Override
    public PollDTO updatePoll(PollDTO pollDTO) {
        Poll poll = new Poll();
        poll.setId(pollDTO.getId());
        poll.setTitle(pollDTO.getTitle());
        poll.setDescription(pollDTO.getDescription());
        poll.setExecutionTime(pollDTO.getExecutionTime());
        poll.setUserRegistrationSecondsBeforeExecution(pollDTO.getUserRegistrationSecondsBeforeExecution());
        poll.setActive(pollDTO.getActive());
        poll.setResultType(pollDTO.getResultType());
        poll = pollRepository.save(poll);
        return new PollDTO(poll);
    }

    @Override
    public void changePollActive(String pollId, Boolean active) {
        Poll poll = pollRepository.findOne(pollId);
        poll.setActive(active);
        pollRepository.save(poll);
    }

    @Override
    public PollDTO getPollById(String pollId) {
        Poll poll = pollRepository.findOne(pollId);
        if (poll != null) {
            return new PollDTO(poll);
        } else {
            throw new RecordNotFoundException("Poll not found");
        }
    }

    @Override
    public PollDTO getNextPoll() {
        Poll poll = pollRepository.findByExecutionTimeGreaterThanAndActiveEqualsOrderByExecutionTimeAsc(System.currentTimeMillis(), true);
        PollDetails pollDetails = null;
        PollDTO pollDTO = null;

        if (poll != null) {
            pollDetails = pollDetailRepository.findByPollIdAndUserId(poll.getId(), authService.getAuthenticatedUserId());
            pollDTO = new PollDTO(poll);
        }

        if (pollDetails != null) {
            pollDTO.setStatus(pollDetails.getStatus() == null ? PollDetails.STATUS_DEFAULT : pollDetails.getStatus());
        }

        return pollDTO;
    }

    @Override
    public List<PollDTO> getAllPolls(Boolean active) {

        List<Poll> polls;

        if (active != null) {
            polls = pollRepository.findByActiveEquals(active);
        } else {
            polls = pollRepository.findAll();
        }

        List<String> pollIds = polls.stream().map(Poll::getId).collect(Collectors.toList());
        List<PollDetails> pollsDetails = pollDetailRepository.findByPollIdInAndUserId(pollIds, authService.getAuthenticatedUserId());

        List<PollDTO> nextPolls = new ArrayList<>();

        polls.forEach(poll -> {
            PollDTO pollDTO = new PollDTO(poll);
            pollDTO.setStatus(getStatusByPollId(pollDTO.getId(), pollsDetails));
            nextPolls.add(pollDTO);
        });

        return nextPolls;
    }

    @Override
    public Page<PollDTO> getNextPolls() {

        PageRequest pageRequest = new PageRequest(0, 50);

        Page<Poll> polls = pollRepository.findByExecutionTimeGreaterThanAndActiveEqualsOrderByExecutionTimeAsc(
                System.currentTimeMillis(),
                true,
                pageRequest
        );

        List<String> pollIds = polls.getContent().stream().map(Poll::getId).collect(Collectors.toList());
        List<PollDetails> pollsDetails = pollDetailRepository.findByPollIdInAndUserId(pollIds, authService.getAuthenticatedUserId());

        List<PollDTO> nextPolls = new ArrayList<>();

        polls.getContent().forEach(poll -> {
            PollDTO pollDTO = new PollDTO(poll);
            pollDTO.setStatus(getStatusByPollId(pollDTO.getId(), pollsDetails));
            pollDTO.setTotalAccepted(pollDetailRepository.countByPollIdAndStatus(poll.getId(), PollDetails.STATUS_ACCEPTED));
            nextPolls.add(pollDTO);
        });

        return new PageImpl<>(nextPolls, pageRequest, nextPolls.size());
    }

    @Override
    public List<UserDTO> getAllUsersForPoll(String pollId) {
        List<String> userIds = pollDetailRepository.findByPollId(pollId).stream().map(PollDetails::getUserId).collect(Collectors.toList());
        return userRepository.findByIdIn(userIds).stream().map(UserDTO::new).collect(Collectors.toList());
    }

    private String getStatusByPollId(String pollId, List<PollDetails> pollsDetails) {
        List<PollDetails> pd = pollsDetails.stream()
                .filter(pollDetails -> pollId.equals(pollDetails.getPollId()))
                .collect(Collectors.toList());

        if (pd != null && !pd.isEmpty()) {
            String status = pd.get(0).getStatus();
            return status == null ? PollDetails.STATUS_DEFAULT : status;
        }
        return PollDetails.STATUS_DEFAULT;
    }

    @Override
    public void deletePoll(String pollId) {
        pollRepository.delete(pollId);
    }

    @Override
    @Scheduled(cron = "*/1 * * * *")
    public void closePollForRegistration() {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);

        List<Poll> polls = pollRepository.getPollsWithClosedRegistration(calendar.getTimeInMillis());

        if (polls != null && !polls.isEmpty()) {
            matrixService.closePollForRegistration();
        }
    }

}
