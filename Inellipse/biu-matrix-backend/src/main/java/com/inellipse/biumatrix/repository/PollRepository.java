package com.inellipse.biumatrix.repository;

import com.inellipse.biumatrix.model.Poll;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.mongodb.repository.MongoRepository;

import java.util.List;

public interface PollRepository extends MongoRepository<Poll, String>, PollRepositoryCustom {
    Poll findByExecutionTimeGreaterThanAndActiveEqualsOrderByExecutionTimeAsc(long time, Boolean active);
    Page<Poll> findByExecutionTimeGreaterThanAndActiveEqualsOrderByExecutionTimeAsc(long time, Boolean active, Pageable pageable);
    List<Poll> findByActiveEquals(Boolean active);
    List<Poll> findByExecutionTimeEquals(long time);
}
