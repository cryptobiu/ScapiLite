package com.inellipse.biumatrix.service;

import com.inellipse.biumatrix.dto.PollDTO;
import com.inellipse.biumatrix.dto.UserDTO;
import org.springframework.data.domain.Page;

import java.util.List;

public interface PollService {
    void savePoll(PollDTO pollDTO);

    PollDTO updatePoll(PollDTO pollDTO);

    void changePollActive(String pollId, Boolean active);

    PollDTO getPollById(String pollId);

    PollDTO getNextPoll();

    List<PollDTO> getAllPolls(Boolean active);

    Page<PollDTO> getNextPolls();

    List<UserDTO> getAllUsersForPoll(String pollId);

    void deletePoll(String pollId);

    void closePollForRegistration();
}
