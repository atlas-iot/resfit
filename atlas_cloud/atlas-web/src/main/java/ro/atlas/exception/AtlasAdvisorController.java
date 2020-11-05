package ro.atlas.exception;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ControllerAdvice;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.context.request.WebRequest;
import org.springframework.web.servlet.mvc.method.annotation.ResponseEntityExceptionHandler;

import java.time.LocalDateTime;
import java.util.LinkedHashMap;
import java.util.Map;

@ControllerAdvice
public class AtlasAdvisorController extends ResponseEntityExceptionHandler {

    @ExceptionHandler(GatewayDuplicateKeyException.class)
    public ResponseEntity<Object> handleGatewayDuplicateKeyException(GatewayDuplicateKeyException e, WebRequest request) {
        Map<String, Object> body = new LinkedHashMap<>();
        body.put("Timestamp", LocalDateTime.now());
        body.put("Message", e.getMessage());
        body.put("DuplicateKey", e.getKey());
        body.put("DuplicateValue", e.getValue());

        return new ResponseEntity<>(body, HttpStatus.CONFLICT);
    }

    @ExceptionHandler(GatewayNotFoundException.class)
    public ResponseEntity<Object> handleGatewayNotFoundException(GatewayNotFoundException e, WebRequest request) {
        Map<String, Object> body = new LinkedHashMap<>();
        body.put("Timestamp", LocalDateTime.now());
        body.put("Message", e.getMessage());

        return new ResponseEntity<>(body, HttpStatus.NOT_FOUND);
    }

    @ExceptionHandler(GatewayNotRegisteredException.class)
    public ResponseEntity<Object> handleGatewayNotRegisteredException(GatewayNotRegisteredException e, WebRequest request) {
        Map<String, Object> body = new LinkedHashMap<>();
        body.put("Timestamp", LocalDateTime.now());
        body.put("Message", e.getMessage());

        return new ResponseEntity<>(body, HttpStatus.METHOD_NOT_ALLOWED);
    }

    @ExceptionHandler(ClientNotFoundException.class)
    public ResponseEntity<Object> handleClientNotFoundException(ClientNotFoundException e, WebRequest request) {
        Map<String, Object> body = new LinkedHashMap<>();
        body.put("Timestamp", LocalDateTime.now());
        body.put("Message", e.getMessage());

        return new ResponseEntity<>(body, HttpStatus.NOT_FOUND);
    }
}
