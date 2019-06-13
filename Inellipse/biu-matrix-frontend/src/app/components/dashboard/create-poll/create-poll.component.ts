import { Component, OnInit, ViewChild } from '@angular/core';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { BsModalRef } from 'ngx-bootstrap/modal';
import { PollService } from '../../../services/poll.service';
import { Poll } from '../../../models/poll.model';
import { NgbDatepicker } from "@ng-bootstrap/ng-bootstrap";

@Component({
  selector: 'app-create-poll',
  templateUrl: './create-poll.component.html',
  styleUrls: ['./create-poll.component.scss']
})
export class CreatePollComponent implements OnInit {

  @ViewChild('dp') public datepicker: NgbDatepicker;
  public createPollForm: FormGroup;
  public datepick;
  public timepick;
  public poll: Poll;
  public error: string;

  constructor(
    public bsModalRef: BsModalRef,
    private formBuilder: FormBuilder,
    private pollService: PollService
  ) {  }

  ngOnInit() {
    this.createPollForm = this.initCreatePollForm();
    if (this.poll) {
      this.populateForm(this.poll);
    }
  }

  public createOrUpdatePoll(form) {
    if (!this.datepick || !this.timepick) {
      this.error = "Date and time are required";
      return;
    }

    let poll: Poll = Object.assign({}, form);
    poll.executionTime = new Date(
      this.datepick.year, this.datepick.month - 1, this.datepick.day,
      this.timepick.hour, this.timepick.minute, 0, 0
    ).getTime();

    if (this.poll) {
      this.updatePoll(poll);
    } else {
      this.createPoll(poll);
    }
  }

  public createPoll(poll: Poll) {
    this.pollService.createPoll(poll).subscribe(res => this.bsModalRef.hide(), err => this.error = err.error.message);
  }

  public updatePoll(poll: Poll) {
    this.pollService.updatePoll(poll.id, poll).subscribe(res => this.bsModalRef.hide(), err => err.error.message);
  }

  public populateForm(poll: Poll) {
    this.createPollForm.patchValue(poll);
    const date = new Date(poll.executionTime);
    this.datepick = { year: date.getFullYear(), month: date.getMonth() + 1, day: date.getDate() };
    this.timepick = { hour: date.getHours(), minute: date.getMinutes() };
    this.datepicker.navigateTo({year: this.datepick.year, month: this.datepick.month});
  }

  private initCreatePollForm(): FormGroup {
    return this.formBuilder.group({
      id: null,
      name: ['', [Validators.required]],
      title: '',
      description: '',
      executionTime: null,
      active: true,
      userRegistrationSecondsBeforeExecution: '',
      resultType: 'basic'
    });
  }

}
