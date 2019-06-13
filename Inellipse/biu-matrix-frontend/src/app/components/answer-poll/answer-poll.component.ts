import { Component, OnInit } from '@angular/core';
import { ActivatedRoute } from "@angular/router";

import { Poll } from '../../models/poll.model';
import { PollService } from '../../services/poll.service';

@Component({
  selector: 'app-answer',
  templateUrl: './answer-poll.component.html',
  styleUrls: ['./answer-poll.component.scss']
})
export class AnswerPollComponent implements OnInit {

  public pollId: string;
  public poll: Poll;
  public pollAnswer: string;

  constructor(
    private pollService: PollService,
    private activatedRoute: ActivatedRoute
  ) {  }

  ngOnInit() {
    this.activatedRoute.params.subscribe(res => {
      this.pollId = res.id;
      this.getPollById(this.pollId);
    });
  }

  answer() {
    console.log(this.pollAnswer);
  }

  getPollById(pollId) {
    this.pollService.getPollById(pollId).subscribe(res => this.poll = res);
  }

}
