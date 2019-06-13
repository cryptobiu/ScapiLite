import { Component, OnInit, TemplateRef, OnDestroy } from '@angular/core';
import { BsModalService, BsModalRef } from 'ngx-bootstrap/modal';
import { CreatePollComponent } from '../create-poll/create-poll.component';
import { PollService } from '../../../services/poll.service';
import { Poll } from '../../../models/poll.model';
import { UserService } from '../../../services/user.service';
import { User } from '../../../models/user.model';

import { Subject } from 'rxjs/Subject';
import 'rxjs/add/operator/takeUntil';

@Component({
  selector: 'app-list-polls',
  templateUrl: './list-polls.component.html',
  styleUrls: ['./list-polls.component.scss']
})
export class ListPollsComponent implements OnInit, OnDestroy {

  public modalRef: BsModalRef;
  public userListModalRef: BsModalRef;
  public polls: Poll[] = [];
  public users: User[] = [];
  public unsubscribe: Subject<void> = new Subject();
  public markPollForDelete: string;

  constructor(
    private modalService: BsModalService,
    private pollService: PollService,
    private userService: UserService
  ) { }

  ngOnInit() {
    this.getPolls();

    this.modalService.onHide.takeUntil(this.unsubscribe).subscribe(res => this.getPolls());
  }

  ngOnDestroy() {
    this.unsubscribe.next();
    this.unsubscribe.complete();
  }

  public openModal(poll: Poll = null) {
    const initialState = { poll };
    this.modalRef = this.modalService.show(CreatePollComponent, { initialState });
  }

  public openConfirmBox(template: TemplateRef<any>, pollId) {
    this.markPollForDelete = pollId;
    this.modalRef = this.modalService.show(template);
  }

  public closePoll(pollId) {
    this.pollService.closePoll(pollId).subscribe(res => this.getPolls(), err => console.error(err));
  }

  confirm() {
    this.deletePoll(this.markPollForDelete);
  }

  decline() {
    this.markPollForDelete = null;
    this.modalRef.hide();
  }

  public deletePoll(pollId) {
    this.pollService.deletePoll(pollId).subscribe(res => this.modalRef.hide());
  }

  public getPolls() {
    this.pollService.getPolls().subscribe(res => this.polls = res);
  }

  public getUsersByPollId(pollId, template: TemplateRef<any>) {
    this.userService.getUsersByPollId(pollId).subscribe(res => {
      this.users = res;
      this.userListModalRef = this.modalService.show(template);
    });
  }

  changeStatus(pollId, pollStatus) {
    this.pollService.changePollActive(pollId, pollStatus).subscribe(res => console.log(res), err => console.error(err));
  }

}
