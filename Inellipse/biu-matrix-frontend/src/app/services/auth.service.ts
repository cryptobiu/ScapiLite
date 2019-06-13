import { Injectable } from '@angular/core';
import { HttpClient, HttpParams, HttpHeaders } from '@angular/common/http';

import { Subject } from 'rxjs/Subject';
import { BehaviorSubject } from 'rxjs/BehaviorSubject';

import { JwtHelperService } from '@auth0/angular-jwt';

import { environment } from '../../environments/environment';

@Injectable()
export class AuthService {

  public loggedIn: Subject<any> = new BehaviorSubject<any>(false);

  constructor(
    private http: HttpClient
  ) { }

  login(data) {
    const headers: HttpHeaders = new HttpHeaders()
      .set('Content-Type', 'application/x-www-form-urlencoded')
      .set('Authorization' , `Basic ${environment.loginToken}`);

    const body: HttpParams = new HttpParams()
      .set('grant_type', 'password')
      .set('username', data.username)
      .set('password', data.password);

    return this.http.post<any>(`${environment.apiUrl}/oauth/token`, body.toString(), { headers });
  }

  public setSession(token) {
    this.setToken(token);
    this.loggedIn.next(true);
  }

  public validateSession() {
    const jwtHelper = new JwtHelperService();
    this.loggedIn.next(!jwtHelper.isTokenExpired(this.getToken()));
  }

  public getToken() {
    return localStorage.getItem('token');
  }

  public setToken(token) {
    localStorage.setItem('token', token.access_token);
  }

}
