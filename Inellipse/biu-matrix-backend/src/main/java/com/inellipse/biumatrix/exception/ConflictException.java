package com.inellipse.biumatrix.exception;

public class ConflictException extends RuntimeException {

    private static final long serialVersionUID = -8629687409002161048L;

    public ConflictException() {
        super();
    }

    public ConflictException(String message) {
        super(message);
    }

}
