package com.inellipse.biumatrix.exception;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.ControllerAdvice;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestController;

import java.util.HashMap;
import java.util.Map;

@ControllerAdvice
@RestController
public class GlobalExceptionHandler {

    private final Logger logger = LoggerFactory.getLogger(getClass());

    private static final int BAD_REQUEST = 400;
    private static final int UNAUTHORIZED = 401;
    private static final int FORBIDDEN = 403;
    private static final int NOT_FOUND = 404;
    private static final int CONFLICT = 409;
    private static final int SERVER_ERROR = 500;

    private static final String RESPONSE_CODE = "code";
    private static final String RESPONSE_MESSAGE = "message";

    @ResponseStatus(HttpStatus.BAD_REQUEST)
    @ExceptionHandler(value = ServiceException.class)
    public Map<String, Object> handleException(ServiceException e) {
        logException(e);
        Map<String, Object> map = new HashMap<>();
        map.put(RESPONSE_CODE, e.getCode());
        map.put(RESPONSE_MESSAGE, e.getMessage());
        return map;
    }

    @ResponseStatus(HttpStatus.BAD_REQUEST)
    @ExceptionHandler(value = BadRequestException.class)
    public Map<String, Object> handleException(BadRequestException e) {
        logException(e);
        Map<String, Object> map = new HashMap<>();
        map.put(RESPONSE_CODE, BAD_REQUEST);
        map.put(RESPONSE_MESSAGE, e.getMessage());
        return map;
    }

    @ResponseStatus(HttpStatus.UNAUTHORIZED)
    @ExceptionHandler(value = UnauthorizedException.class)
    public Map<String, Object> handleException(UnauthorizedException e) {
        logException(e);
        Map<String, Object> map = new HashMap<>();
        map.put(RESPONSE_CODE, UNAUTHORIZED);
        map.put(RESPONSE_MESSAGE, e.getMessage());
        return map;
    }

    @ResponseStatus(HttpStatus.FORBIDDEN)
    @ExceptionHandler(value = AccessDeniedException.class)
    public Map<String, Object> handleException(AccessDeniedException e) {
        logException(e);
        Map<String, Object> map = new HashMap<>();
        map.put(RESPONSE_CODE, FORBIDDEN);
        map.put(RESPONSE_MESSAGE, e.getMessage());
        return map;
    }

    @ResponseStatus(HttpStatus.NOT_FOUND)
    @ExceptionHandler(value = RecordNotFoundException.class)
    public Map<String, Object> handleException(RecordNotFoundException e) {
        logException(e);
        Map<String, Object> map = new HashMap<>();
        map.put(RESPONSE_CODE, NOT_FOUND);
        map.put(RESPONSE_MESSAGE, e.getMessage());
        return map;
    }

    @ResponseStatus(HttpStatus.CONFLICT)
    @ExceptionHandler(value = ConflictException.class)
    public Map<String, Object> handleException(ConflictException e) {
        logException(e);
        Map<String, Object> map = new HashMap<>();
        map.put(RESPONSE_CODE, CONFLICT);
        map.put(RESPONSE_MESSAGE, e.getMessage());
        return map;
    }

    @ResponseStatus(HttpStatus.INTERNAL_SERVER_ERROR)
    @ExceptionHandler(value = Exception.class)
    public Map<String, Object> handleException(Exception e) {
        logException(e);
        Map<String, Object> map = new HashMap<>();
        map.put(RESPONSE_CODE, SERVER_ERROR);
        map.put(RESPONSE_MESSAGE, e.getMessage());
        return map;
    }

    private void logException(Exception e) {
        logger.error("Exception", e);
    }
}