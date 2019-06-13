package com.inellipse.biumatrix.controller;

import com.inellipse.biumatrix.dto.PollDTO;
import com.inellipse.biumatrix.dto.UserDTO;
import com.inellipse.biumatrix.service.PollDetailService;
import com.inellipse.biumatrix.service.PollService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Page;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
public class PollController {

    private PollService pollService;
    private PollDetailService pollDetailService;

    @Autowired
    public PollController(PollService pollService, PollDetailService pollDetailService) {
        this.pollService = pollService;
        this.pollDetailService = pollDetailService;
    }

    @PostMapping(value = "/polls")
    public void savePoll(@RequestBody PollDTO pollDTO) {
        pollService.savePoll(pollDTO);
    }

    @PostMapping(value = "/polls/details")
    public PollDTO savePollDetails(@RequestParam String pollId,
                                   @RequestParam String userId,
                                   @RequestParam(required = false) String status) {
        return pollDetailService.savePollDetails(pollId, userId, status);
    }

    @PutMapping(value = "/polls/{pollId}")
    public PollDTO updatePoll(@RequestBody PollDTO pollDTO) {
        return pollService.updatePoll(pollDTO);
    }

    @PostMapping(value = "/polls/{pollId}")
    public void changePollActive(@PathVariable("pollId") String pollId, @RequestParam Boolean active) {
        pollService.changePollActive(pollId, active);
    }

    @GetMapping(value = "/external/polls/{pollId}")
    public PollDTO getPollById(@PathVariable("pollId") String pollId) {
        return pollService.getPollById(pollId);
    }

    @GetMapping(value = "/polls")
    public List<PollDTO> getAllPolls(@RequestParam(value = "active", required = false) Boolean active) {
        return pollService.getAllPolls(active);
    }

    @GetMapping(value = "/polls/next")
    public PollDTO getNextPoll() {
        return pollService.getNextPoll();
    }

    @GetMapping(value = "/polls/next/pageable")
    public Page<PollDTO> getNextPolls() {
        return pollService.getNextPolls();
    }

    @GetMapping(value = "/polls/{pollId}/users")
    public List<UserDTO> getAllUsersForPoll(@PathVariable("pollId") String pollId) {
        return pollService.getAllUsersForPoll(pollId);
    }

    @DeleteMapping(value = "/polls/{pollId}")
    public void deletePoll(@PathVariable("pollId") String pollId) {
        pollService.deletePoll(pollId);
    }

}
