package com.inellipse.biumatrix.repository;

import com.inellipse.biumatrix.model.Poll;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.aggregation.Aggregation;
import org.springframework.data.mongodb.core.aggregation.AggregationResults;
import org.springframework.data.mongodb.core.query.Criteria;

import java.util.List;

public class PollRepositoryImpl implements PollRepositoryCustom {

    private static final String COLLECTION_NAME = "poll";
    private final MongoTemplate mongoTemplate;

    public PollRepositoryImpl(MongoTemplate mongoTemplate) {
        this.mongoTemplate = mongoTemplate;
    }

    @Override
    public List<Poll> getPollsWithClosedRegistration(long time) {

        Aggregation aggregation = Aggregation.newAggregation(
                Aggregation.project("name").andExpression("executionTime - (userRegistrationSecondsBeforeExecution * 1000)").as("difference"),
                Aggregation.match(new Criteria("difference").is(time)));


        AggregationResults<Poll> aggregationResults = mongoTemplate.aggregate(aggregation, COLLECTION_NAME, Poll.class);

        return aggregationResults.getMappedResults();
    }
}
