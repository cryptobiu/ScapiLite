import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { CreatePollComponent } from './create-poll/create-poll.component';
import { ListPollsComponent } from './list-polls/list-polls.component';
import { ListUsersComponent } from './list-users/list-users.component';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { ModalModule } from 'ngx-bootstrap/modal';
import { NgbModule } from '@ng-bootstrap/ng-bootstrap';
import { RouterModule, Routes } from '@angular/router';
import { LayoutComponent } from './layout/layout.component';
import { NavbarComponent } from '../../core/navbar/navbar.component';

const routes: Routes = [
  {
    path: '', component: LayoutComponent, children: [
      { path: '', redirectTo: 'polls', pathMatch: 'full' },
      { path: 'polls', component: ListPollsComponent },
      { path: 'users', component: ListUsersComponent }
    ]
  },
];

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    ReactiveFormsModule,
    ModalModule.forRoot(),
    NgbModule.forRoot(),
    RouterModule.forChild(routes)
  ],
  declarations: [
    CreatePollComponent,
    ListPollsComponent,
    ListUsersComponent,
    LayoutComponent,
    NavbarComponent
  ],
  entryComponents: [
    CreatePollComponent
  ]
})
export class DashboardModule { }
