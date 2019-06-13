package com.inellipse.biumatrix.service.impl;

import com.inellipse.biumatrix.dto.PollDTO;
import com.inellipse.biumatrix.model.PollDetails;
import com.inellipse.biumatrix.repository.PollDetailRepository;
import com.inellipse.biumatrix.repository.PollRepository;
import com.inellipse.biumatrix.service.PollDetailService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class PollDetailServiceImpl implements PollDetailService {

    private PollDetailRepository pollDetailRepository;
    private PollRepository pollRepository;

    @Autowired
    public PollDetailServiceImpl(PollDetailRepository pollDetailRepository, PollRepository pollRepository) {
        this.pollDetailRepository = pollDetailRepository;
        this.pollRepository = pollRepository;
    }

    @Override
    public PollDTO savePollDetails(String pollId, String userId, String status) {
        PollDetails pollDetails = pollDetailRepository.findByPollIdAndUserId(pollId, userId);
        if (pollDetails != null) {
            pollDetails.setStatus(status);
        } else {
            pollDetails = new PollDetails();
            pollDetails.setPollId(pollId);
            pollDetails.setUserId(userId);
            pollDetails.setStatus(status);
        }

        pollDetailRepository.save(pollDetails);

        PollDTO pollDTO = new PollDTO(pollRepository.findOne(pollId));
        pollDTO.setStatus(status);
        return pollDTO;
    }
}
