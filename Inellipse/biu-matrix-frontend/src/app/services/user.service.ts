import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs/Observable';
import { environment } from '../../environments/environment';
import { User } from '../models/user.model';

@Injectable()
export class UserService {

  constructor(
    private http: HttpClient
  ) { }

  getUsersByPollId(pollId): Observable<User[]> {
    return this.http.get<User[]>(`${environment.apiUrl}/polls/${pollId}/users`);
  }

  getUsers(): Observable<User[]> {
    return this.http.get<User[]>(`${environment.apiUrl}/users`);
  }

}
