package com.inellipse.biumatrix.repository;

import com.inellipse.biumatrix.model.PollDetails;
import org.springframework.data.mongodb.repository.MongoRepository;

import java.util.List;

public interface PollDetailRepository extends MongoRepository<PollDetails, String> {
    PollDetails findByPollIdAndUserId(String pollId, String userId);
    List<PollDetails> findByPollIdInAndUserId(List<String> pollIds, String userId);
    List<PollDetails> findByPollId(String pollId);
    long countByPollIdAndStatus(String pollId, String status);
}
