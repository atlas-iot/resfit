package ro.atlas.configuration;

import java.util.concurrent.Executor;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.scheduling.annotation.EnableAsync;
import org.springframework.scheduling.concurrent.ThreadPoolTaskExecutor;

@Configuration
@EnableAsync(proxyTargetClass = true)
public class AtlasAsyncConfiguration {
	private static final Logger LOG = LoggerFactory.getLogger(AtlasAsyncConfiguration.class);

	private static final String ATLAS_TASK_EXECUTOR_PREFIX = "ATLAS_TASK_EXECUTOR_PREFIX-";
	private static final int ATLAS_TASK_QUEUE_MAX_CAPACITY = 128;
	private static final int ATLAS_TASK_EXECUTOR_CORE_POOL_SIZE = 16;
	private static final int ATLAS_TASK_EXECUTOR_MAX_POOL_SIZE = 32;
	
	@Bean (name = "taskExecutor")
	public Executor taskExecutor() {
		LOG.info("Create Task Executor");
		final ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
		/* Static number of threads which are always alive */
		executor.setCorePoolSize(ATLAS_TASK_EXECUTOR_CORE_POOL_SIZE);
		
		executor.setMaxPoolSize(ATLAS_TASK_EXECUTOR_MAX_POOL_SIZE);
		executor.setQueueCapacity(ATLAS_TASK_QUEUE_MAX_CAPACITY);
		
		executor.setThreadNamePrefix(ATLAS_TASK_EXECUTOR_PREFIX);
		executor.initialize();
		
		return executor;
	}
}
