package ro.atlas;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.context.properties.EnableConfigurationProperties;
import org.springframework.boot.web.servlet.support.SpringBootServletInitializer;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.data.mongodb.repository.config.EnableMongoRepositories;

import ro.atlas.properties.AtlasProperties;

@ComponentScan(basePackages = {"ro.atlas"})
@EnableMongoRepositories(basePackages = {"ro.atlas.repository"})
@EnableConfigurationProperties({
	AtlasProperties.class
})
@SpringBootApplication
public class CloudApplication extends SpringBootServletInitializer {
	
	public static void main(String[] args) {
		SpringApplication.run(CloudApplication.class, args);
	}
}
