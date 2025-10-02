import { NestFactory } from '@nestjs/core';
import { SwaggerModule, DocumentBuilder } from '@nestjs/swagger';
import { AppModule } from './app.module';

async function bootstrap() {
  const app = await NestFactory.create(AppModule);

  const config = new DocumentBuilder()
      .setTitle('FuturEd AI Bootstrap API')
      .setDescription('The FuturEd AI API description')
      .setVersion('1.0')
      .build();
  const documentFactory = () => SwaggerModule.createDocument(app, config);
  SwaggerModule.setup('schema', app, documentFactory);

  await app.listen(process.env.PORT ?? 3000);
}
bootstrap();
