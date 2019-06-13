import { Component, OnInit } from '@angular/core';
import { FormGroup, FormBuilder } from '@angular/forms';
import { Router } from '@angular/router';
import { AuthService } from "../../services/auth.service";

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.scss']
})
export class LoginComponent implements OnInit {

  public loginForm: FormGroup;
  public error: string;

  constructor(
    private formBuilder: FormBuilder,
    private router: Router,
    private auth: AuthService
  ) { }

  ngOnInit() {
    this.loginForm = this.initLoginForm();
  }

  login(form) {
    this.auth.login(form).subscribe(res => {
      this.error = null;
      this.auth.setSession(res);
      this.router.navigate(['/']);
    }, err => this.error = err.error.error_description);
  }

  private initLoginForm(): FormGroup {
    return this.formBuilder.group({
      username: null,
      password: null
    });
  }

}
