import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Observable } from 'rxjs/Observable';
import { environment } from '../../environments/environment';
import { Poll } from '../models/poll.model';

@Injectable()
export class PollService {

  constructor(
    private http: HttpClient
  ) { }

  createPoll(poll: Poll) {
    return this.http.post<any>(`${environment.apiUrl}/polls`, poll);
  }

  changePollActive(pollId, active) {

    const headers: HttpHeaders = new HttpHeaders()
      .set('Content-Type', 'application/x-www-form-urlencoded');

    const body: HttpParams = new HttpParams()
      .set('active', active);

    return this.http.post<any>(`${environment.apiUrl}/polls/${pollId}`, body.toString(), { headers });
  }

  updatePoll(pollId, poll: Poll) {
    return this.http.put<any>(`${environment.apiUrl}/polls/${pollId}`, poll);
  }

  getPollById(pollId) {
    return this.http.get<Poll>(`${environment.apiUrl}/external/polls/${pollId}`);
  }

  getPolls(): Observable<Poll[]> {
    return this.http.get<Poll[]>(`${environment.apiUrl}/polls`);
  }

  deletePoll(pollId) {
    return this.http.delete<any>(`${environment.apiUrl}/polls/${pollId}`);
  }

  closePoll(pollId) {
    return this.http.get<any>(`${environment.apiUrl}/polls/${pollId}/close`);
  }
}
