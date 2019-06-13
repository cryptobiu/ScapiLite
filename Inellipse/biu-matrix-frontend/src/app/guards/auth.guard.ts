import { Injectable } from '@angular/core';
import { CanActivate, ActivatedRouteSnapshot, RouterStateSnapshot, Router } from '@angular/router';
import { Observable } from 'rxjs/Observable';
import { JwtHelperService } from "@auth0/angular-jwt";
import { AuthService } from "../services/auth.service";

@Injectable()
export class AuthGuard implements CanActivate {

  constructor(private router: Router, private auth: AuthService) {
  }

  canActivate(
    next: ActivatedRouteSnapshot,
    state: RouterStateSnapshot): Observable<boolean> | Promise<boolean> | boolean {

      const jwtHelper = new JwtHelperService();

      if (jwtHelper.isTokenExpired(this.auth.getToken())) {
        this.router.navigate(['/login']);
        return false;
      } else {
        return true;
      }
  }
}
