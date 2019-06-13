import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { HttpClientModule, HTTP_INTERCEPTORS } from '@angular/common/http';

import { ModalModule } from 'ngx-bootstrap/modal';
import { NgbModule } from '@ng-bootstrap/ng-bootstrap';

import { AppRoutingModule } from './app-routing.module';

import { AppComponent } from './app.component';
import { PollService } from './services/poll.service';
import { UserService } from './services/user.service';
import { LoginComponent } from './components/login/login.component';
import { AuthGuard } from './guards/auth.guard';
import { AuthService } from "./services/auth.service";
import { TokenInterceptor } from './interceptors/token.interceptor';
import { InstructionComponent } from './components/instruction/instruction.component';
import { AnswerPollComponent } from './components/answer-poll/answer-poll.component';

@NgModule({
  declarations: [
    AppComponent,
    LoginComponent,
    InstructionComponent,
    AnswerPollComponent
  ],
  imports: [
    BrowserModule,
    HttpClientModule,
    FormsModule,
    ReactiveFormsModule,
    AppRoutingModule
  ],
  providers: [
    PollService,
    UserService,
    AuthGuard,
    AuthService,
    {
      provide: HTTP_INTERCEPTORS,
      useClass: TokenInterceptor,
      multi: true
    }
  ],
  bootstrap: [
    AppComponent
  ]
})
export class AppModule { }
