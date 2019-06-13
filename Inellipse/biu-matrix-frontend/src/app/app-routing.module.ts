import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { LoginComponent } from './components/login/login.component';
import { AuthGuard } from './guards/auth.guard';

import { InstructionComponent } from './components/instruction/instruction.component';
import { AnswerPollComponent } from "./components/answer-poll/answer-poll.component";

const routes: Routes = [
  { path: '', loadChildren: './components/dashboard/dashboard.module#DashboardModule', canActivate: [AuthGuard] },
  { path: 'poll/:id', component: AnswerPollComponent  },
  { path: 'privateInstance', component: InstructionComponent },
  { path: 'login', component: LoginComponent }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
